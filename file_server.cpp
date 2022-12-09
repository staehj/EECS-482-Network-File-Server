#include "file_server.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

#include "fs_server.h"
#include "helpers.h"


FileServer::FileServer(int port_number) : block_locks(FS_DISKSIZE) {
    // traverse from root (block 0)
    traverse_fs();

    // run server
    port = port_number;
    if (run(30) == -1) {
        std::cerr << "Error: run failed\n";
        exit(1);
    }
}

int FileServer::run(int queue_size) {
	// (1) Create socket
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		perror("Error opening stream socket");
		return -1;
	}

	// (2) Set the "reuse port" socket option
	int yesval = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yesval, sizeof(yesval)) == -1) {
		perror("Error setting socket options");
		return -1;
	}

	// (3) Create a sockaddr_in struct for the proper port and bind() to it.
	struct sockaddr_in addr;
	if (make_server_sockaddr(&addr, port) == -1) {
		return -1;
	}

	// (3b) Bind to the port.
	if (bind(sock, (sockaddr *) &addr, sizeof(addr)) == -1) {
		perror("Error binding stream socket");
		return -1;
	}

	// (3c) Detect which port was chosen.
	port = get_port_number(sock);

    // NETWORK SET UP COMPLETE
    // Broadcast FileServer boot
    cout_lock.lock();
    std::cout << "\n@@@ port " << port << std::endl;
    cout_lock.unlock();

	// (4) Begin listening for incoming connections.
	listen(sock, queue_size);

	// (5) Serve incoming connections one by one forever.
	while (true) {
		int connectionfd = accept(sock, 0, 0);
		if (connectionfd == -1) {
			perror("Error accepting connection");
			return -1;
		}

        ThreadRAII(this, connectionfd);
	}
}

void FileServer::thread_start(int connectionfd) {
    // start handle_message through thread
    try {
        if (handle_message(connectionfd) == -1) {
            perror("Error: handle_connection failed\n");
            throw Exception();
        }
    }
    catch (Exception &exception) {
        close(connectionfd);
    }
}

int FileServer::handle_message(int connectionfd) {
	// initialize buffer
    char buf[MAX_MESSAGE_SIZE];
	memset(buf, 0, sizeof(buf));

    // receive into buf (until null character is found)
    int buf_len = receive_until_null(connectionfd, buf);
    std::string request = std::string(buf);

    // parse
    RequestType req_type = parse(request);

    // check if request type is write
    if (req_type == RequestType::WRITE) {
        // save remaining input to data
        char data[FS_BLOCKSIZE];
        memset(data, 0, sizeof(data));

        int data_len = buf_len - (request.length() + 1);
        for (int i = 0; i < data_len; ++i) {
            data[i] = buf[request.length() + 1 + i];
        }

        // receive_data
        if ((unsigned int) data_len < FS_BLOCKSIZE) {
            receive_data(connectionfd, data_len, data);
        }

        handle_request(req_type, request, data, connectionfd);
    }
    else {
        handle_request(req_type, request, nullptr, connectionfd);
    }

	// close connection
	close(connectionfd);

	return 0;
}

FileServer::RequestType FileServer::parse(std::string msg) {
    std::stringstream msg_ss(msg);
    std::string canonical;
    std::string word;
    RequestType request_type;

    msg_ss >> word;
    canonical += word;

    if (word == "FS_READBLOCK") {
        request_type = RequestType::READ;
        canonical += " " + check_username(msg_ss);
        canonical += " " + check_pathname(msg_ss);
        canonical += " " + check_block(msg_ss);
    }
    else if (word == "FS_WRITEBLOCK") {
        request_type = RequestType::WRITE;
        canonical += " " + check_username(msg_ss);
        canonical += " " + check_pathname(msg_ss);
        canonical += " " + check_block(msg_ss);
    }
    else if (word == "FS_CREATE") {
        request_type = RequestType::CREATE;
        canonical += " " + check_username(msg_ss);
        canonical += " " + check_pathname(msg_ss);
        canonical += " " + check_type(msg_ss);
    }
    else if (word == "FS_DELETE") {
        request_type = RequestType::DELETE;
        canonical += " " + check_username(msg_ss);
        canonical += " " + check_pathname(msg_ss);
    }
    else {
        // invalid
        std::cerr << "Error: invalid request: " << word << "\n";
        throw Exception();
    }

    if (msg == canonical) {
        return request_type;
    }
    else {
        // invalid
        std::cerr << "Error: canonical comparison failed:\n" << msg << "\n";
        throw Exception();
    }
}

std::string FileServer::check_username(std::stringstream &msg_ss) {
    std::string username;
    if (msg_ss >> username) {
        if (username.length() > FS_MAXUSERNAME) {
            std::cerr << "Error: long username: " << username << "\n";
            throw Exception();
        }
    }
    else {
        std::cerr << "Error: missing username\n";
        throw Exception();
    }
    return username;
}

std::string FileServer::check_pathname(std::stringstream &msg_ss) {
    std::string pathname;
    if (msg_ss >> pathname) {
        if (pathname.length() > FS_MAXPATHNAME
        || pathname[0] != '/' || pathname[pathname.length()-1] == '/') {
            std::cerr << "Error: invalid pathname: " << pathname << "\n";
            throw Exception();
        }
    }
    else {
        std::cerr << "Error: missing pathname\n";
        throw Exception();
    }
    return pathname;
}

std::string FileServer::check_block(std::stringstream &msg_ss) {
    int block;
    if (msg_ss >> block) {
        if (block < 0 || (unsigned int) block >= FS_DISKSIZE) {
            std::cerr << "Error: invalid block: " << block << "\n";
            throw Exception();
        }
    }
    else {
        std::cerr << "Error: missing block\n";
        throw Exception();
    }
    return std::to_string(block);
}

std::string FileServer::check_type(std::stringstream &msg_ss) {
    char type;
    if (msg_ss >> type) {
        if (type != 'f' && type != 'd') {
            std::cerr << "Error: invalid type: " << type << "\n";
            throw Exception();
        }
    }
    else {
        std::cerr << "Error: missing type\n";
        throw Exception();
    }
    return std::string(1, type);
}

// TODO: Remove debug couts before submit
void FileServer::handle_request(RequestType type, std::string request, const char* data,
                                int connectionfd) {
    // std::cout << "------------------------------------\n";
    // std::cout << "REQUEST: " << request << '\n';
    switch(type) {
        case RequestType::READ:
            assert(data == nullptr);
            handle_read(request, connectionfd);
            break;
        case RequestType::WRITE:
            handle_write(request, data, connectionfd);
            break;
        case RequestType::CREATE:
            assert(data == nullptr);
            handle_create(request, connectionfd);
            break;
        case RequestType::DELETE:
            assert(data == nullptr);
            handle_delete(request, connectionfd);
            break;
        default:
            std::cerr << "Error: this should never print...\n";
            throw Exception();
    }
}

void FileServer::handle_read(std::string request, int connectionfd) {
    std::stringstream req_ss(request);
    std::string req_type, username, pathname;
    int block;
    req_ss >> req_type >> username >> pathname >> block;

    // decompose path
    std::deque<std::string> names;
    decompose_path(names, pathname);

    // check it exists
    std::unique_lock<std::mutex> cur_lock(block_locks[0]);
    fs_inode cur_inode;
    find_path(names, username, cur_lock, cur_inode);

    // check that inode is file
    check_inode_type(cur_inode, 'f');

    // check that block is valid
    if (block < 0 || (uint32_t) block >= cur_inode.size) {
        std::cerr << "Error: block " << block << " was out of range\n";
        throw Exception();
    }

    // read block
    char data[FS_BLOCKSIZE];
    disk_readblock(cur_inode.blocks[block], data);

    // send request + data to client
    send_bytes(connectionfd, request.c_str(), request.size() + 1);
    send_bytes(connectionfd, data, FS_BLOCKSIZE);
}

void FileServer::handle_write(std::string request, const char* data, int connectionfd) {
    std::stringstream req_ss(request);
    std::string req_type, username, pathname;
    int block;
    req_ss >> req_type >> username >> pathname >> block;

    // decompose path
    std::deque<std::string> names;
    decompose_path(names, pathname);


    // check it exists
    std::unique_lock<std::mutex> cur_lock(block_locks[0]);
    fs_inode cur_inode;
    int cur_block = find_path(names, username, cur_lock, cur_inode);
    // check that inode is file
    check_inode_type(cur_inode, 'f');

    // check that block is valid
    if (block < 0 || (uint32_t) block > cur_inode.size) {
        std::cerr << "Error: block " << block << " was out of range\n";
        throw Exception();
    }

    // writing to block immediately after end of file
    if ((uint32_t) block == cur_inode.size) {
        // check inode size is less than FS_MAXFILEBLOCKS
        if (cur_inode.size >= FS_MAXFILEBLOCKS) {
            std::cerr << "Error: inode ran out of blocks (>124)\n";
            throw Exception();
        }

        // get free block
        int new_block = get_free_block();

        // write data from input to free block (disk)
        disk_writeblock(new_block, data);

        // increment size
        ++cur_inode.size;


        // add new block number to inode blocks
        cur_inode.blocks[block] = new_block;

        // write inode to disk
        disk_writeblock(cur_block, &cur_inode);
    }
    else {
        // write data from input to given block (disk)
        disk_writeblock(cur_inode.blocks[block], data);
    }

    // send request to client
    send_bytes(connectionfd, request.c_str(), request.size() + 1);
}

void FileServer::handle_create(std::string request, int connectionfd) {
    std::stringstream req_ss(request);
    std::string req_type, username, pathname;
    char type;
    req_ss >> req_type >> username >> pathname >> type;

    // decompose path
    std::deque<std::string> names;
    decompose_path(names, pathname);

    if (names.empty()) {
        std::cerr << "Error: user tried to create root directory\n";
        throw Exception();
    }

    // remove last inode name
    std::string new_name = names.back();
    names.pop_back();

    // check it exists
    std::unique_lock<std::mutex> cur_lock(block_locks[0]);
    fs_inode cur_inode;
    int cur_block = find_path(names, username, cur_lock, cur_inode);

    // check that inode is directory
    check_inode_type(cur_inode, 'd');

    fs_direntry block_direntries [FS_DIRENTRIES];
    // zero initialize new
    for (int i = 0; i < FS_DIRENTRIES; ++i) {
        fs_direntry zero;
        zero.inode_block = 0;
        block_direntries[i] = zero;
    }

    // check if file/dir with name new_name already exists
    DirEntryIndex direntry_ind = check_name_exists_get_free_direntry(cur_inode, new_name,
                                                                     block_direntries);

    std::cout << "DirEntryIndex info:\n";
    std::cout << "block: " << direntry_ind.block << '\n';
    std::cout << "block_index: " << direntry_ind.block_index << '\n';
    std::cout << "direntry_offset: " << direntry_ind.direntry_offset << '\n';

    // create new thing (file or directory)
    create_inode(cur_inode, cur_block, username, new_name, type, direntry_ind, block_direntries);

    // send request to client
    send_bytes(connectionfd, request.c_str(), request.size() + 1);
}

void FileServer::handle_delete(std::string request, int connectionfd) {
    std::stringstream req_ss(request);
    std::string req_type, username, pathname;
    req_ss >> req_type >> username >> pathname;

    // decompose path
    std::deque<std::string> names;
    decompose_path(names, pathname);

    if (names.empty()) {
        std::cerr << "Error: user tried to delete root directory\n";
        throw Exception();
    }

    // remove last inode name
    std::string new_name = names.back();
    names.pop_back();

    // check it exists
    std::unique_lock<std::mutex> cur_lock(block_locks[0]);
    fs_inode cur_inode;
    int cur_block = find_path(names, username, cur_lock, cur_inode);

    // get target inode block number
    DirEntryIndex direntry_index;
    fs_direntry buf_direntries [FS_DIRENTRIES];
    // zero initialize new
    for (int i = 0; i < FS_DIRENTRIES; ++i) {
        fs_direntry zero;
        zero.inode_block = 0;
        buf_direntries[i] = zero;
    }
    int target_inode_block = get_target_inode_block(cur_inode, new_name, direntry_index,
                                                    buf_direntries);

    // grab lock of target inode
    std::unique_lock<std::mutex> target_lock(block_locks[target_inode_block]);

    // read inode from disk
    fs_inode target_inode;
    disk_readblock(target_inode_block, &target_inode);

    // if directory, it must be empty
    if (target_inode.type == 'd') {
        check_directory_empty(target_inode);
    }

    // check user permissions of delete target
    check_inode_username(target_inode, username);

    // free all blocks used by target_inode
    // Note: if directory, this will not be entered
    for (int i = 0; (uint32_t) i < target_inode.size; ++i) {
        add_free_block(target_inode.blocks[i]);
    }

    // free target_inode block
    // UPDATES inode block in disk
    add_free_block(target_inode_block);

    // check if it is the only direntry in block
    if (get_direntry_count(buf_direntries) == 1) {
        // align blocks to left + decrement size
        fix_inode(cur_inode, direntry_index.block_index);

        // update free blocks
        add_free_block(direntry_index.block);

        // write new cur_inode to disk
        disk_writeblock(cur_block, &cur_inode);
    }
    // there are many direntries in direntry block
    // UPDATES direntries block in disk
    else {
        // remove direntry from direntries block
        buf_direntries[direntry_index.direntry_offset].inode_block = 0;

        // update new direntries block in disk
        disk_writeblock(direntry_index.block, buf_direntries);
    }

    // send request to client
    send_bytes(connectionfd, request.c_str(), request.size() + 1);
}

int FileServer::find_path(std::deque<std::string> &names, std::string username,
                           std::unique_lock<std::mutex> &cur_lock, fs_inode &cur_inode) {
    // temporary buffers
    fs_direntry buf_direntries [FS_DIRENTRIES];

    // get root inode
    // Note: we are holding lock for root inode
    disk_readblock(0, &cur_inode);

    // root directory
    if (names.empty()) {
        return 0;
    }

    int next_inode;
    bool found;
    while (!names.empty()) {
        std::string cur_name = names.front();
        names.pop_front();

        // check inode is dir, not file
        if (!names.empty()) {
            check_inode_type(cur_inode, 'd');
        }

        found = false;
        // search block in cur_inode for direntry with name cur_name
        for (int i = 0; (uint32_t) i < cur_inode.size; ++i) {
            int cur_block = cur_inode.blocks[i];

            // read direntries from block
            disk_readblock(cur_block, &buf_direntries);

            // store all inode blocks from direntries
            for (int j = 0; (unsigned int) j < FS_DIRENTRIES; ++j) {
                // unused directory
                if (buf_direntries[j].inode_block == 0) {
                    continue;
                }

                // check if cur_name is found
                if (std::string(buf_direntries[j].name) == cur_name) {
                    // get lock of this inode
                    next_inode = buf_direntries[j].inode_block;
                    std::unique_lock<std::mutex> next_lock(block_locks[next_inode]);
                    cur_lock.swap(next_lock);

                    // update cur_inode
                    disk_readblock(next_inode, &cur_inode);

                    // check user permissions for inode
                    check_inode_username(cur_inode, username);

                    found = true;
                    break;
                }
            }

            if (found) {
                break;
            }
        }
        // CHANGED FROM SUBMIT
        // if (!found) {
        //     std::cerr << "Error: pathname not found\n";
        //     throw Exception();
        // }
    }

    if (found) {
        return next_inode;
    }

    std::cerr << "Error: pathname not found\n";
    throw Exception();
}

void FileServer::decompose_path(std::deque<std::string> &names, std::string pathname) {
    std::string name;
    for (size_t i = 1; i < pathname.length(); i++) {
        if (pathname[i] != '/') {
            name += pathname[i];
        }
        else {
            if (name == "") {
                std::cerr << "Error: path had consecutive /s\n";
                throw Exception();
            }
            if (name.length() > FS_MAXFILENAME) {
                std::cerr << "Error: file name too long (>59)\n";
                throw Exception();
            }
            names.push_back(name);
            name = "";
        }
    }
    if (name.length() > FS_MAXFILENAME) {
        std::cerr << "Error: file name too long (>59)\n";
        throw Exception();
    }
    names.push_back(name);
}

void FileServer::traverse_fs() {
    // temporary buffers
    fs_inode buf_inode;
    fs_direntry buf_direntries [FS_DIRENTRIES];

    // initialize free_blocks_set to all blocks
    std::unordered_set<uint32_t> free_blocks_set;
    for (uint32_t i = 0; i < FS_DISKSIZE; ++i) {
        free_blocks_set.insert(i);
    }

    std::queue<uint32_t> next_inodes;
    next_inodes.push(0);
    while (!next_inodes.empty()) {
        // get next inode number
        uint32_t target = next_inodes.front();
        next_inodes.pop();
        free_blocks_set.erase(target);

        // read inode
        disk_readblock(target, &buf_inode);

        // parse all direntries blocks from inode
        for (uint32_t i = 0; i < buf_inode.size; ++i) {
            target = buf_inode.blocks[i];

            // cur inode is directory
            if (buf_inode.type == 'd') {
                // read direntries
                disk_readblock(target, &buf_direntries);

                // store all inode blocks from direntries
                for (unsigned int j = 0; j < FS_DIRENTRIES; ++j) {
                    // unused directory
                    if (buf_direntries[j].inode_block == 0) {
                        continue;
                    }
                    next_inodes.push(buf_direntries[j].inode_block);
                }
            }
            // cur inode is file
            else {
                free_blocks_set.erase(target);
            }
        }
    }

    for (uint32_t block_num : free_blocks_set) {
        free_blocks.push(block_num);
    }
}

void FileServer::check_inode_type(fs_inode &cur_inode, char type) {
    if (cur_inode.type != type) {
        std::cerr << "Error: inode was type " << cur_inode.type << ", client sent "
                  << type << '\n';
        throw Exception();
    }
}

void FileServer::check_inode_username(fs_inode &cur_inode, std::string username) {
    if (std::string(cur_inode.owner) != username && std::string(cur_inode.owner) != "") {
        std::cerr << "Error: inode had owner " << cur_inode.owner << ", client sent "
                  << username << '\n';
        throw Exception();
    }
}

DirEntryIndex FileServer::check_name_exists_get_free_direntry(fs_inode &inode, std::string name, fs_direntry* block_direntries) {
    // first free direntry index
    fs_direntry buf_direntries [FS_BLOCKSIZE];
    // zero initialize new
    for (int i = 0; i < FS_DIRENTRIES; ++i) {
        fs_direntry zero;
        zero.inode_block = 0;
        block_direntries[i] = zero;
    }

    DirEntryIndex free_direntry_index(-1, -1, -1);
    bool free_found = false;
    
    // iterate through blocks
    for (int i = 0; (uint32_t) i < inode.size; ++i) {
        int block = inode.blocks[i];

        // read direntries from block
        disk_readblock(block, buf_direntries);

        // store all inode blocks from direntries
        for (int j = 0; (unsigned int) j < FS_DIRENTRIES; ++j) {
            // unused directory
            if (buf_direntries[j].inode_block == 0) {
                if (!free_found) {
                    // copy buf_direntries into block_direntries
                    for (int i = 0; (unsigned int) i < FS_DIRENTRIES; ++i) {
                        block_direntries[i] = buf_direntries[i];
                    }

                    free_found = true;
                    free_direntry_index = DirEntryIndex(i, j, block);
                }
                continue;
            }

            // check if cur_name is found
            if (std::string(buf_direntries[j].name) == name) {
                std::cerr << "Error: in CREATE, name " << name << " already existed\n";
                throw Exception();
            }
        }
    }
    return free_direntry_index;
}


void FileServer::create_inode(fs_inode &cur_inode, int cur_block, std::string username,
                              std::string name, char type, DirEntryIndex ind, fs_direntry* block_direntries) {
    // get free space for a direntry
    bool cur_inode_changed = false;

    // check if new block needs to be created for direntry
    if (ind.block == -1) {
        ind.block_index = cur_inode.size;
        ind.direntry_offset = 0;
        ind.block = get_free_block();
    }

    // new block was created for direntry
    if (cur_inode.size == (uint32_t) ind.block_index) {
        // check inode size is less than FS_MAXFILEBLOCKS
        if (cur_inode.size >= FS_MAXFILEBLOCKS) {
            std::cerr << "Error: inode ran out of blocks (>124)\n";
            throw Exception();
        }

        cur_inode_changed = true;

        // if new block assigned, link cur_inode to new block
        cur_inode.blocks[ind.block_index] = ind.block;
        cur_inode.size++;

        // zero initialize new
        for (int i = 0; i < FS_DIRENTRIES; ++i) {
            fs_direntry zero;
            zero.inode_block = 0;
            block_direntries[i] = zero;
        }
    }

    // create new inode
    fs_inode new_inode;
    new_inode.type = type;
    strcpy(new_inode.owner, username.c_str());
    new_inode.size = 0;

    // write inode to disk
    int inode_block = get_free_block();
    disk_writeblock(inode_block, &new_inode);

    // add new direntry
    fs_direntry new_direntry;
    strcpy(new_direntry.name, name.c_str());
    new_direntry.inode_block = inode_block;
    block_direntries[ind.direntry_offset] = new_direntry;

    // write new direntry to disk
    disk_writeblock(ind.block, block_direntries);

    // write cur_inode to disk if changed
    if (cur_inode_changed) {
        disk_writeblock(cur_block, &cur_inode);
    }
}

int FileServer::get_free_block() {
    if (free_blocks.empty()) {
        std::cerr << "Error: no free block remaining\n";
        throw Exception();
    }

    int free_block = free_blocks.top();
    free_blocks.pop();
    return free_block;
}

void FileServer::add_free_block(int block) {
    free_blocks.push(block);
}

int FileServer::get_target_inode_block(fs_inode &inode, std::string name,
                                       DirEntryIndex &direntry_ind,
                                       fs_direntry* buf_direntries) {
    // iterate through blocks
    for (int i = 0; (uint32_t) i < inode.size; ++i) {
        int block = inode.blocks[i];

        // read direntries from block
        disk_readblock(block, buf_direntries);

        // store all inode blocks from direntries
        for (int j = 0; (unsigned int) j < FS_DIRENTRIES; ++j) {
            // unused directory
            if (buf_direntries[j].inode_block == 0) {
                continue;
            }

            // check if cur_name is found
            if (std::string(buf_direntries[j].name) == name) {
                direntry_ind.block_index = i;
                direntry_ind.direntry_offset = j;
                direntry_ind.block = block;
                return buf_direntries[j].inode_block;
            }
        }
    }

    // not found
    std::cerr << "Error: in DELETE, " << name << " does not exist\n";
    throw Exception();
}

void FileServer::check_directory_empty(fs_inode &inode) {
    assert(inode.type == 'd');

    if (inode.size != 0) {
        std::cerr << "Error: delete target was not empty\n";
        throw Exception();
    }
}

int FileServer::get_direntry_count(fs_direntry* buf_direntries) {
    int count = 0;
    for (unsigned int i = 0; i < FS_DIRENTRIES; ++i) {
        if (buf_direntries[i].inode_block != 0) {
            ++count;
        }
    }
    assert(count != 0);
    return count;
}

void FileServer::fix_inode(fs_inode &inode, int target) {
    // decrement inode size
    --inode.size;
    for (uint32_t i = target; i < inode.size; ++i) {
        inode.blocks[i] = inode.blocks[i+1];
    }
}

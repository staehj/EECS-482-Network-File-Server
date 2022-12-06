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
        std::cout << "Error: run failed\n";
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

        // start handle_message through thread
        try {
            // std::thread t(&FileServer::thread_start, this, connectionfd, sock);
            // t.detach();
            ThreadRAII(this, connectionfd, sock);
        }
        catch (Exception &exception) {
            close(connectionfd);
        }

	}
}

void FileServer::thread_start(int connectionfd, int sock) {
    if (handle_message(connectionfd, sock) == -1) {
        perror("Error: handle_connection failed\n");
        throw Exception();
    }
}

int FileServer::handle_message(int connectionfd, int sock) {
	// initialize buffer
    char buf[MAX_MESSAGE_SIZE];
	memset(buf, 0, sizeof(buf));

    // receive into buf (until null character is found)
    // TODO: make exceptions and put this into try-catch
    int buf_len = receive_until_null(connectionfd, sock, buf);
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
        receive_data(connectionfd, sock, data_len, data);

        handle_request(req_type, request, data, sock);
    }
    else {
        handle_request(req_type, request, nullptr, sock);
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
        if (block < 0 || block >= FS_DISKSIZE) {
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

void FileServer::handle_request(RequestType type, std::string request, const char* data,
                                int sock) {
    switch(type) {
        case RequestType::READ:
            assert(data == nullptr);
            handle_read(request, sock);
            break;
        case RequestType::WRITE:
            handle_write(request, data, sock);
            break;
        case RequestType::CREATE:
            assert(data == nullptr);
            handle_create(request, sock);
            break;
        case RequestType::DELETE:
            assert(data == nullptr);
            handle_delete(request, sock);
            break;
        default:
            std::cerr << "Error: this should never print...\n";
            throw Exception();
    }
}

void FileServer::handle_read(std::string request, int sock) {
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
    
    // check user permissions for inode
    check_inode_username(cur_inode, username);

    // iterate through data blocks
    for (int i = 0; i < cur_inode.size; ++i) {
        // read data from block
        char data[FS_BLOCKSIZE];
        disk_readblock(cur_inode.blocks[i], data);

        // send request + data to client
        send_bytes(sock, request.c_str());
        send_bytes(sock, data);
    }
}

void FileServer::handle_write(std::string request, const char* data, int sock) {

}

void FileServer::handle_create(std::string request, int sock) {
    std::stringstream req_ss(request);
    std::string req_type, username, pathname;
    char type;
    req_ss >> req_type >> username >> pathname >> type;

    // decompose path
    std::deque<std::string> names;
    decompose_path(names, pathname);

    // remove last inode name
    std::string new_name = names.back();
    names.pop_back();

    // check it exists
    std::unique_lock<std::mutex> cur_lock(block_locks[0]);
    fs_inode cur_inode;
    int cur_block = find_path(names, username, cur_lock, cur_inode);

    // check user permissions for inode
    check_inode_username(cur_inode, username);

    // check that inode is directory
    check_inode_type(cur_inode, 'd');

    // check if file/dir with name new_name already exists
    if (check_name_exists_in_dir(cur_inode, new_name)) {
        std::cerr << "Error: in CREATE, name " << new_name << " already existed in "
                  << pathname << '\n';
        throw Exception();
    }

    // create new thing (file or directory)
    create_inode(cur_inode, cur_block, username, new_name, type);

    // send request to client
    send_bytes(sock, request.c_str());
}

void FileServer::handle_delete(std::string request, int sock) {

}

int FileServer::find_path(std::deque<std::string> &names, std::string username,
                           std::unique_lock<std::mutex> &cur_lock, fs_inode &cur_inode) {
    // temporary buffers
    fs_inode buf_inode;
    fs_direntry buf_direntries [FS_DIRENTRIES];

    // get root inode
    // Note: we are holding lock for root inode
    disk_readblock(0, &cur_inode);
    bool found = false;

    while (!names.empty()) {
        std::string cur_name = names.front();
        names.pop_front();

        // check user has inode permissions
        if (std::string(cur_inode.owner) != ""
        && std::string(cur_inode.owner) != username) {
            std::cerr << "Error: user " << username << " permission denied, owner was "
                      << cur_inode.owner << "\n";
            throw Exception();
        }

        // check inode is dir, not file
        check_inode_type(cur_inode, 'd');

        // search block in cur_inode for direntry with name cur_name
        for (int i = 0; i < cur_inode.size; ++i) {
            int cur_block = cur_inode.blocks[i];

            // read direntries from block
            disk_readblock(cur_block, &buf_direntries);

            // store all inode blocks from direntries
            for (int j = 0; j < FS_DIRENTRIES; ++j) {
                // unused directory
                if (buf_direntries[j].inode_block == 0) {
                    break;
                }

                // check if cur_name is found
                if (std::string(buf_direntries[j].name) == cur_name) {
                    // get lock of this inode
                    int next_inode = buf_direntries[j].inode_block;
                    std::unique_lock<std::mutex> next_lock(block_locks[next_inode]);
                    cur_lock.swap(next_lock);

                    // update cur_inode
                    disk_readblock(next_inode, &cur_inode);

                    return next_inode;
                }
            }
        }
    }

    std::cerr << "Error: pathname not found\n";
    throw Exception();
}

void FileServer::decompose_path(std::deque<std::string> &names, std::string pathname) {
    std::string name;
    for (int i = 1; i < pathname.length(); i++) {
        if (pathname[i] != '/') {
            name += pathname[i];
        }
        else {
            names.push_back(name);
            name = "";
        }
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

        // parse all direntries from inode
        for (uint32_t i = 0; i < buf_inode.size; ++i) {
            target = buf_inode.blocks[i];

            // cur inode is directory
            if (buf_inode.type == 'd') {
                // read direntries
                disk_readblock(target, &buf_direntries);

                // store all inode blocks from direntries
                for (int j = 0; j < FS_DIRENTRIES; ++j) {
                    // unused directory
                    if (buf_direntries[j].inode_block == 0) {
                        break;
                    }
                    next_inodes.push(buf_direntries[j].inode_block);
                }
            }
            // cur inode is file
            else {
                next_inodes.push(target);
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
    if (std::string(cur_inode.owner) != username) {
        std::cerr << "Error: inode had owner " << cur_inode.owner << ", client sent "
                  << username << '\n';
        throw Exception();
    }
}

bool FileServer::check_name_exists_in_dir(fs_inode &inode, std::string name) {
    fs_direntry buf_direntries [FS_DIRENTRIES];
    // iterate through blocks
    for (int i = 0; i < inode.size; ++i) {
        int block = inode.blocks[i];

        // read direntries from block
        disk_readblock(block, &buf_direntries);

        // store all inode blocks from direntries
        for (int j = 0; j < FS_DIRENTRIES; ++j) {
            // unused directory
            if (buf_direntries[j].inode_block == 0) {
                break;
            }

            // check if cur_name is found
            if (std::string(buf_direntries[j].name) == name) {
                return true;
            }
        }
    }
    return false;
}


void FileServer::create_inode(fs_inode &cur_inode, int cur_block, std::string username,
                              std::string name, char type) {
    // get free space for a direntry
    DirEntryIndex ind = get_free_direntryindex(cur_inode);
    fs_direntry buf_direntries [FS_DIRENTRIES];
    bool cur_inode_changed = false;

    // new block was created for direntry
    if (cur_inode.size == ind.block_index) {
        cur_inode_changed = true;

        // if new block assigned, link cur_inode to new block
        cur_inode.blocks[ind.block_index] = ind.block;
        cur_inode.size++;

        // zero initialize new 
        memset(buf_direntries, 0, sizeof(buf_direntries));
    }
    // existing block was used
    else {
        // Note: block contains direntries
        disk_readblock(ind.block, buf_direntries);
    }

    // create new inode
    fs_inode new_inode;
    new_inode.type = type;
    strcpy(new_inode.owner, username.c_str());
    new_inode.size = 0;

    // write inode to disk
    int ionde_block = get_free_block();
    disk_writeblock(ionde_block, &new_inode);

    // add new direntry
    fs_direntry new_direntry;
    strcpy(new_direntry.name, name.c_str());
    new_direntry.inode_block = ionde_block;
    buf_direntries[ind.direntry_offset] = new_direntry;

    // write new direntry to disk
    disk_writeblock(ind.block, &buf_direntries);

    // write cur_inode to disk if changed
    if (cur_inode_changed) {
        disk_writeblock(cur_block, &cur_inode);
    }
}

DirEntryIndex FileServer::get_free_direntryindex(fs_inode inode) {
    fs_direntry buf_direntries [FS_DIRENTRIES];
    // iterate through blocks
    for (int i = 0; i < inode.size; ++i) {
        int block = inode.blocks[i];

        // read direntries from block
        disk_readblock(block, &buf_direntries);

        // store all inode blocks from direntries
        for (int j = 0; j < FS_DIRENTRIES; ++j) {
            // unused directory
            if (buf_direntries[j].inode_block == 0) {
                return DirEntryIndex(i, j, block);
            }
        }
    }

    int free_block = get_free_block();

    return DirEntryIndex(free_block, 0, free_block);
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

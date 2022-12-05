#include "file_server.h"

#include <cassert>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <thread>

#include "fs_server.h"
#include "helpers.h"


FileServer::FileServer(int port_number) {
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
        
        //// debugging ////
        std::cerr << "Client says " << request << std::endl;
        
        for (int i = 0; i < data_len; ++i) {
            std::cerr << data[i];
        }
        std::cerr << '\n';
        ////////


        handle_request(req_type, request, data);
    }
    else {
        handle_request(req_type, request, nullptr);
    }

    // send to client
    send_bytes(sock, request.c_str());

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
        std::cerr << "Error: missing username.\n";
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
        std::cerr << "Error: missing pathname.\n";
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
        std::cerr << "Error: missing block.\n";
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
        std::cerr << "Error: missing type.\n";
        throw Exception();
    }
    return std::string(1, type);
}

void FileServer::handle_request(RequestType type, std::string request, const char* data) {
    // TODO maybe some RAII stuff for exception/error handling
    switch(type) {
        case RequestType::READ:
            assert(data == nullptr);
            handle_read(request);
            break;
        case RequestType::WRITE:
            handle_write(request, data);
            break;
        case RequestType::CREATE:
            assert(data == nullptr);
            handle_create(request);
            break;
        case RequestType::DELETE:
            assert(data == nullptr);
            handle_delete(request);
            break;
        default:
            std::cerr << "Error: this should never print...\n";
            throw Exception();
    }
}

void FileServer::handle_read(std::string request) {
    std::stringstream req_ss(request);
    std::string req_type, username, pathname;
    int block;
    req_ss >> req_type >> username >> pathname >> block;

    // check it exists
    check_path_exists(pathname);

    // read from disk

}

void FileServer::handle_write(std::string request, const char* data) {

}

void FileServer::handle_create(std::string request) {

}

void FileServer::handle_delete(std::string request) {

}

void FileServer::traverse_fs() {
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
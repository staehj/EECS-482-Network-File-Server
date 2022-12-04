#include "file_server.h"

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <string>

#include "exception.h"
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


/**
 * Receives a string message from the client and prints it to stdout.
 *
 * Parameters:
 * 		connectionfd: 	File descriptor for a socket connection
 * 				(e.g. the one returned by accept())
 * Returns:
 *		0 on success, -1 on failure.
 */
int handle_connection(int connectionfd, int sock) {

	printf("New connection %d\n", connectionfd);

	char buf[MAX_MESSAGE_SIZE];
	memset(buf, 0, sizeof(buf));

	char data[FS_BLOCKSIZE];
	memset(data, 0, sizeof(data));

    // try {
    //     receive_bytes(connectionfd, sock, msg);
    // }
    // catch (CUSTOMECEPTION) {
    //     exit(1)
    // }
    int buf_len = receive_until_null(connectionfd, sock, buf);
    std::string request = std::string(buf);

    // if (!parse(request)) {
    //     // handle error: request was invalid
    // }





    // parse


    // check if request type is write
        // save remaining input to data
        int data_len = buf_len - (request.length() + 1);
        for (int i = 0; i < data_len; ++i) {
            data[i] = buf[request.length() + 1 + i];
        }    

        // receive_data
        receive_data(connectionfd, sock, data_len, data);

    
    // debugging
    std::cout << "Client says " << request << std::endl;
    
    for (int i = 0; i < data_len; ++i) {
        std::cout << data[i];
    }
    std::cout << '\n';

    //


    // // if parsed msg is valid, send to client
    // send_bytes(sock, msg.c_str());

	// (4) Close connection
	close(connectionfd);

	return 0;
}

/**
 * Endlessly runs a server that listens for connections and serves
 * them _synchronously_.
 *
 * Parameters:
 *		port: 		The port on which to listen for incoming connections.
 *		queue_size: 	Size of the listen() queue
 * Returns:
 *		-1 on failure, does not return on success.
 */
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

		if (handle_connection(connectionfd, sock) == -1) {
            perror("Error: handle_cnnection failed\n");
			return -1;
		}
	}
}


void FileServer::handle_request(RequestType type) {
    // TODO maybe some RAII stuff for exception/error handling
    switch(type) {
        case RequestType::READ:
            handle_read();
            break;
        case RequestType::WRITE:
            handle_write();
            break;
        case RequestType::CREATE:
            handle_create();
            break;
        case RequestType::DELETE:
            handle_delete();
            break;
        default:
            std::cout << "Error: handle_request got invalid RequestType\n";
            exit(1);
    }
}

void FileServer::handle_read() {

}

void FileServer::handle_write() {

}

void FileServer::handle_create() {

}

void FileServer::handle_delete() {

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
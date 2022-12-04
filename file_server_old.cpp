#include "file_server.h"

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include "exception.h"
#include "fs_server.h"


FileServer::FileServer(int port_number) {

    // traverse from root (block 0)
    traverse_fs();

    // Ohjun's TODO: set up socket to connect to fs (include listen)
    struct sockaddr_in addr;
    int sock;
    int opt = 1;
    socklen_t addr_len = sizeof(addr);

    // socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        // handle error
        perror("Error: socket failed\n");
        exit(1);
    }

    // setsockopt
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        // handle error
        perror("Error: setsockopt failed\n");
        exit(1);
    }

    // bind
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (port_number == 0) {
        // let OS decide
        addr.sin_port = htons(port_number);
    }
    else {
        addr.sin_port = port_number;
    }
    // addr.sin_port = htons(port_number);

    // if (port_number == -1) {
    //     if (getsockname(sock, (struct sockaddr*) &addr, &addr_len) < 0) {
    //         // handle error
    //         perror("Error: getsockname failed\n");
    //         exit(1);
    //     }
    //     port_number = ntohs(addr.sin_port);
    // }
    // else {
    //     // addr.sin_port = htons(port_number);÷
    //     addr.sin_port = port_number;
    // }

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        // handle error
        perror("Error: bind failed\n");
        exit(1);
    }

    if (port_number == 0) {
        // TODO: check if ntohs is used correctly here
        if ((port_number = ntohs(getsockname(sock, (struct sockaddr*) &addr, &addr_len))) < 0) {
            // handle error
            perror("Error: getsockname failed\n");
            exit(1);
        }
    }

    // listen
    if (listen(sock, 30) < 0) {
        // handle error
        perror("Error: listen failed\n");
        exit(1);
    }

    // NETWORK SET UP COMPLETE
    // Broadcast FileServer boot
    std::cout << "\n@@@ port " << addr.sin_port << std::endl;

    // accept
    // int new_socket;
    // while ((new_socket = accept(sock, (struct sockaddr*)&addr, &addr_len)) >= 0) {
    //     std::cout << "hello world\n";
    //     // get, parse, handle client request

    //     // figure out where we need to start a new thread

    //     // figure out locks for shared buffers, or just don't have shared buffers
    // }

    while(true) {
        int new_socket;
        if ((new_socket = accept(sock, (struct sockaddr*)&addr, &addr_len)) < 0) {
            perror("Error: Accept failed\n");
            exit(1);
        }

        std::cout << "asdf\n";
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
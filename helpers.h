#ifndef _HELPERS_H_
#define _HELPERS_H_

#include <arpa/inet.h>		// htons(), ntohs()
#include <iostream>
#include <mutex>
#include <netdb.h>		// gethostbyname(), struct hostent
#include <netinet/in.h>		// struct sockaddr_in
#include <stdio.h>		// perror(), fprintf()
#include <string.h>		// memcpy()
#include <sys/socket.h>		// getsockname()
#include <thread>
#include <unistd.h>		// stderr
#include <vector>

#include "fs_server.h"


extern const size_t MAX_MESSAGE_SIZE;

struct DirEntryIndex {
    int block_index;
    int direntry_offset;
    int block;

    DirEntryIndex() {};

    DirEntryIndex(int block_index, int direntry_offset, int block)
        : block_index(block_index), direntry_offset(direntry_offset), block(block) {};
};

int make_server_sockaddr(struct sockaddr_in *addr, int port);

int make_client_sockaddr(struct sockaddr_in *addr, const char *hostname, int port);

int get_port_number(int sockfd);

int send_bytes(int sock, const char* msg, size_t message_len);

int receive_until_null(int connectionfd, int sock, char* buf);

void receive_data(int connectionfd, int sock, int data_len, char* data);

#endif /* _HELPERS_H_ */

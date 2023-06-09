#include "helpers.h"

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

#include "file_server.h"
#include "fs_server.h"


const size_t MAX_MESSAGE_SIZE
    = sizeof("FS_WRITEBLOCK") + FS_MAXUSERNAME + FS_MAXPATHNAME
        + sizeof(int) + 4 + FS_BLOCKSIZE;

int make_server_sockaddr(struct sockaddr_in *addr, int port) {
	// Step (1): specify socket family.
	// This is an internet socket.
	addr->sin_family = AF_INET;

	// Step (2): specify socket address (hostname).
	// The socket will be a server, so it will only be listening.
	// Let the OS map it to the correct address.
	addr->sin_addr.s_addr = INADDR_ANY;

	// Step (3): Set the port value.
	// If port is 0, the OS will choose the port for us.
	// Use htons to convert from local byte order to network byte order.
	addr->sin_port = htons(port);

	return 0;
}

int make_client_sockaddr(struct sockaddr_in *addr, const char *hostname, int port) {
	// Step (1): specify socket family.
	// This is an internet socket.
	addr->sin_family = AF_INET;

	// Step (2): specify socket address (hostname).
	// The socket will be a client, so call this unix helper function
	// to convert a hostname string to a useable `hostent` struct.
	struct hostent *host = gethostbyname(hostname);
	if (host == nullptr) {
		fprintf(stderr, "%s: unknown host\n", hostname);
		return -1;
	}
	memcpy(&(addr->sin_addr), host->h_addr, host->h_length);

	// Step (3): Set the port value.
	// Use htons to convert from local byte order to network byte order.
	addr->sin_port = htons(port);

	return 0;
}

int get_port_number(int sockfd) {
    struct sockaddr_in addr;
    socklen_t length = sizeof(addr);
    if (getsockname(sockfd, (sockaddr *) &addr, &length) == -1) {
        cout_lock.lock();
        std::cout << "Error getting port of socket\n";
        cout_lock.unlock();
        return -1;
    }
    // Use ntohs to convert from network byte order to host byte order.
    return ntohs(addr.sin_port);
}

int send_bytes(int connectionfd, const char* msg, size_t message_len) {
	size_t sent = 0;
	do {
		ssize_t n = send(connectionfd, msg + sent, message_len - sent, MSG_NOSIGNAL);
        if (n == -1) {
            cout_lock.lock();
            std::cout << "Error: send failed\n";
            cout_lock.unlock();
            // break;  // ignore send failure as per piazza
            throw Exception(); // close connection as per piazza @1594
        }
        sent += n;
	} while (sent < message_len);

    return 0;
}

int receive_until_null(int connectionfd, char* buf) {
    // Call recv() enough times to consume all the data the client sends.
    size_t recvd = 0;
    ssize_t rval;
    int i = 0;
    do {
        // Receive as many additional bytes as we can in one call to recv()
        // (while not exceeding MAX_MESSAGE_SIZE bytes in total).
        rval = recv(connectionfd, buf + recvd, MAX_MESSAGE_SIZE - recvd, 0);
        if (rval == -1) {
            cout_lock.lock();
            std::cout << "Error reading stream message\n";
            cout_lock.unlock();
            throw Exception();
        }

        recvd += rval;

        if (recvd > MAX_MESSAGE_SIZE) {
            cout_lock.lock();
            std::cout << "Error: received more bytes than maximum allowed before NULL\n";
            cout_lock.unlock();
            throw Exception();
        }
        for (; (size_t) i < recvd; ++i) {
            if (buf[i] == '\0') {
                return recvd;
            }
        }

	} while (rval > 0);  // recv() returns 0 when client closes

    // TODO: think about if this is an error case
    return recvd;
}

void receive_data(int connectionfd, int data_len, char* data) {
    // Receive as many additional bytes as we can in one call to recv()
    // (while not exceeding MAX_MESSAGE_SIZE bytes in total).
    ssize_t rval = recv(connectionfd, data + data_len, FS_BLOCKSIZE - data_len,
                        MSG_WAITALL);
    if (rval == 0) {
        cout_lock.lock();
        std::cout << "Client closed before sending all data\n";
        cout_lock.unlock();
        throw Exception();
    }
    else if (rval == -1) {
        cout_lock.lock();
        std::cout << "Error reading stream message\n";
        cout_lock.unlock();
        throw Exception();
    }
}

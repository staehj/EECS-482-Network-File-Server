#include <arpa/inet.h>		// htons(), ntohs()
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


struct DirEntryIndex {
    int block_index;
    int direntry_offset;
    int block;

    DirEntryIndex(int block_index, int direntry_offset, int block)
        : block_index(block_index), direntry_offset(direntry_offset), block(block) {};
};

class ThreadRAII {
public:
    ThreadRAII(FileServer* fs, int connectionfd, int sock) {
        t = std::thread(&FileServer::thread_start, fs, connectionfd, sock);
    }

    ~ThreadRAII() {
        t.detach();
    }

private:
    std::thread t;
};

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
        perror("Error getting port of socket");
        return -1;
    }
    // Use ntohs to convert from network byte order to host byte order.
    return ntohs(addr.sin_port);
}

int send_bytes(int sock, const char* msg) {
	// Call send() enough times to send all the data
	size_t message_len = strlen(msg) + 1;
	// size_t message_len = strlen(msg);
	size_t sent = 0;
    std::cout << "msg" << msg << '\n';
	do {
		ssize_t n = send(sock, msg + sent, message_len - sent, MSG_NOSIGNAL);
        if (n == -1) {
            continue;
        }
    std::cout << "sent before: " << sent << '\n';
    std::cout << "n: " << n << '\n';
		sent += n;
    std::cout << "sent after: " << sent << '\n';
	} while (sent < message_len);

    std::cout << "meslen: " << message_len << '\n';
    std::cout << "sent: " << sent << '\n';
    return 0;
}

int receive_until_null(int connectionfd, int sock, char* buf) {
    // Call recv() enough times to consume all the data the client sends.
    size_t recvd = 0;
    ssize_t rval;
    int i = 0;
    do {
        // Receive as many additional bytes as we can in one call to recv()
        // (while not exceeding MAX_MESSAGE_SIZE bytes in total).
        rval = recv(connectionfd, buf + recvd, MAX_MESSAGE_SIZE - recvd, 0);
        if (rval == -1) {
            perror("Error reading stream message");
            // TODO: handle error
        }

        recvd += rval;

        if (recvd > MAX_MESSAGE_SIZE) {
            // TODO: handle error: invalid
        }
        for (; i < recvd; ++i) {
            if (buf[i] == '\0') {
                return recvd;
            }
        }

	} while (rval > 0);  // recv() returns 0 when client closes

    // TODO: think about if this is an error case
    return recvd;
}

void receive_data(int connectionfd, int sock, int data_len, char* data) {
    // Receive as many additional bytes as we can in one call to recv()
    // (while not exceeding MAX_MESSAGE_SIZE bytes in total).
    ssize_t rval = recv(connectionfd, data + data_len, FS_BLOCKSIZE - data_len, MSG_WAITALL);
    if (rval == 0) {
        perror("Client closed before sending all data\n");
        // TODO: handle error
    }
    else if (rval == -1) {
        perror("Error reading stream message");
        // TODO: handle error
    }
}

#include <arpa/inet.h>		// htons(), ntohs()
#include <netdb.h>		// gethostbyname(), struct hostent
#include <netinet/in.h>		// struct sockaddr_in
#include <stdio.h>		// perror(), fprintf()
#include <string.h>		// memcpy()
#include <sys/socket.h>		// getsockname()
#include <unistd.h>		// stderr

#include <string>

/**
 * Make a server sockaddr given a port.
 * Parameters:
 *		addr: 	The sockaddr to modify (this is a C-style function).
 *		port: 	The port on which to listen for incoming connections.
 * Returns:
 *		0 on success, -1 on failure.
 * Example:
 *		struct sockaddr_in server;
 *		int err = make_server_sockaddr(&server, 8888);
 */
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

/**
 * Make a client sockaddr given a remote hostname and port.
 * Parameters:
 *		addr: 		The sockaddr to modify (this is a C-style function).
 *		hostname: 	The hostname of the remote host to connect to.
 *		port: 		The port to use to connect to the remote hostname.
 * Returns:
 *		0 on success, -1 on failure.
 * Example:
 *		struct sockaddr_in client;
 *		int err = make_client_sockaddr(&client, "141.88.27.42", 8888);
 */
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

/**
 * Return the port number assigned to a socket.
 *
 * Parameters:
 * 		sockfd:	File descriptor of a socket
 *
 * Returns:
 *		The port number of the socket, or -1 on failure.
 */
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
	size_t message_len = strlen(msg);
	size_t sent = 0;
	do {
		ssize_t n = send(sock, msg + sent, message_len - sent, MSG_NOSIGNAL);
		if (n == -1) {
            // TODO handle error
			perror("Error sending on stream socket");
			// return -1;
		}
		sent += n;
	} while (sent < message_len);

    return 0;
}

std::string receive_bytes(int connectionfd, int sock) {
    // (1) Receive message from client.
	char buf[MAX_MESSAGE_SIZE];
	memset(buf, 0, sizeof(buf));

	// Call recv() enough times to consume all the data the client sends.
    std::string msg;
	size_t recvd = 0;
	ssize_t rval;
    int i = 0;
	do {
		// Receive as many additional bytes as we can in one call to recv()
		// (while not exceeding MAX_MESSAGE_SIZE bytes in total).
        std::cout << "1\n";
		rval = recv(connectionfd, buf, MAX_MESSAGE_SIZE, 0);
        std::cout << "2\n";
        if (rval == -1) {
			perror("Error reading stream message");
            // TODO: handle error
		}
		recvd += rval;
        if (recvd > MAX_MESSAGE_SIZE) {
            // TODO: handle error: invalid
        }

        msg += std::string(buf, rval);
	} while (rval > 0);  // recv() returns 0 when client closes

    return msg;
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

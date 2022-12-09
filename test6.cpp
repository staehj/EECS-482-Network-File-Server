#include <iostream>
#include <cassert>
#include <cstdlib>
#include <stdlib.h>
#include "fs_client.h"

#include <string>

using std::cout;

int main(int argc, char *argv[]) {
    char *server;
    int server_port;

    const char *writedata1 = "In this project, you will implement a multi-threaded network file server. Clients that use your file server will interact with it via network messages. This project will help you understand hierarchical file systems, socket programming, client-server systems, and network protocols, and it will give you experience building a substantial multi-threaded program with fine-grained locking.In this project, you will implement a multi-threaded network file server. Clients that use your file server will interactDONE";
    const char *writedata2 = "In this project, you will implement a multi-threaded network file server. Clients that use your file server will interact with it via network messages. This project will help you understand hierarchical file systems, socket programming, client-server systems, and network protocols, and it will give you experience building a substantial multi-threaded program with fine-grained locking.In this project, you will implement a multi-threaded network file server. Clients that use your file server will interactDONEDONEDONEDONE";

    char readdata[FS_BLOCKSIZE];
    int status;

    if (argc != 3) {
        cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    fs_create("user1", "/" , 'd');

    fs_create("user1", "//" , 'd');

    fs_create("user1", "///" , 'd');

    fs_delete("user1", "/");

    fs_delete("user1", "//");

    fs_delete("user1", "///");
}

#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

using std::cout;

int main(int argc, char *argv[]) {
    char *server;
    int server_port;

    const char *writedata = "In this project, you will implement a multi-threaded network file server.\0 Clients that use your file server will interact with it via network messages.\0 This project will help you understand hierarchical file systems, socket programming, client-server systems, and network protocols, and it will give you experience building a substantial multi-threaded program with fine-grained locking\0. Since clients are untrusted, your file server should be careful in how it handles network input.\0 Avoid making assumptions about the content and size of the requests until you have verified those assumptions.";

    char readdata[FS_BLOCKSIZE];
    int status;

    if (argc != 3) {
        cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    fs_create("user1", "/dir", 'd');
    fs_create("user1", "/dir/file", 'f');

    fs_create("user1", "/dir/file/invalid", 'f');
    fs_create("user1", "/dir/file/invalid", 'd');

    fs_delete("user1", "/dir/file/invalid");

    fs_writeblock("user1", "/dir/file", 0, writedata);
    fs_writeblock("user1", "/dir/file", 1, writedata);
    fs_readblock("user1", "/dir/", 0, readdata);

    fs_create("user1", "/dir/file/invalid", 'f');
    fs_create("user1", "/dir/file/invalid", 'd');

    fs_delete("user1", "/dir/file/invalid");
}

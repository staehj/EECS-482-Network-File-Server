#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

using std::cout;

int main(int argc, char *argv[]) {
    char *server;
    int server_port;

    const char *writedata_contains_null = "In this project, you will implement a multi-threaded network file server.\0 Clients that use your file server will interact with it via network messages.\0 This project will help you understand hierarchical file systems, socket programming, client-server systems, and network protocols, and it will give you experience building a substantial multi-threaded program with fine-grained locking\0. Since clients are untrusted, your file server should be careful in how it handles network input.\0 Avoid making assumptions about the content and size of the requests until you have verified those assumptions.";

    char readdata[FS_BLOCKSIZE];
    int status;

    if (argc != 3) {
        cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    fs_create("user1", "/dir1", 'd');
    fs_create("user1", "/dir2", 'd');
    fs_create("user1", "/dir3", 'd');
    fs_create("user1", "/dir4", 'd');
    fs_create("user1", "/dir5", 'd');
    fs_create("user1", "/dir6", 'd');
    fs_create("user1", "/dir7", 'd');
    fs_create("user1", "/dir8", 'd');

    fs_create("user1", "/dir9", 'd');
    fs_create("user1", "/dir10", 'd');
    fs_create("user1", "/dir11", 'd');
    fs_create("user1", "/dir12", 'd');
    fs_create("user1", "/dir13", 'd');
    fs_create("user1", "/dir14", 'd');
    fs_create("user1", "/dir15", 'd');
    fs_create("user1", "/dir16", 'd');

    fs_create("user1", "/dir17", 'd');

    fs_delete("user1", "/dir9");
    fs_delete("user1", "/dir10");
    fs_delete("user1", "/dir11");
    fs_delete("user1", "/dir12");
    fs_delete("user1", "/dir13");
    fs_delete("user1", "/dir14");
    fs_delete("user1", "/dir15");
    fs_delete("user1", "/dir16");

    fs_create("user1", "/dir17/file", 'f');
}
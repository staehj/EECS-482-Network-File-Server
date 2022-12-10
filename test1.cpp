#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

using std::cout;

int main(int argc, char *argv[]) {
    char *server;
    int server_port;

    const char *writedata = "In this project, you will implement a multi-threaded network file server. Clients that use your file server will interact with it via network messages. This project will help you understand hierarchical file systems, socket programming, client-server systems, and network protocols, and it will give you experience building a substantial multi-threaded program with fine-grained locking.";

    char readdata[FS_BLOCKSIZE];
    int status;

    if (argc != 3) {
        cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    status = fs_create("user1", "/dir", 'd');
    // assert(!status);

    // parsing error: / at end of path
    status = fs_create("user1", "/dir/file/", 'f');
    // assert(!status);

    // parsing error: / not at start of path
    status = fs_create("user1", "dir/file", 'f');
    // assert(!status);

    // parsing error: username length exceeded
    status = fs_create("user1user1user1user1user1user1", "dir/file", 'f');
    // assert(!status);

    status = fs_create("user1", "/dir/file", 'f');
    // assert(!status);

    // error: different username same directory name
    status = fs_create("user2", "/dir/file", 'f');
    // assert(!status);

    // error: write request to root
    status = fs_writeblock("user1", "/dir", 0, writedata);
    // assert(!status);

    status = fs_writeblock("user1", "/dir/file", 0, writedata);
    // assert(!status);

    // error: read block offset out of range
    status = fs_readblock("user1", "/dir/file", 1, readdata);
    // assert(!status);

    // error: write request block offset out of range
    status = fs_writeblock("user1", "/dir/file", 2, writedata);
    // assert(!status);

    // error: write request w/o user permission
    status = fs_writeblock("user2", "/dir/file", 0, writedata);
    // assert(!status);

    status = fs_writeblock("user1", "/dir/file", 0, writedata);
    // assert(!status);

    status = fs_writeblock("user1", "/dir/file", 1, writedata);
    // assert(!status);

    // error: read request w/o user permission
    status = fs_readblock("user2", "/dir/file", 0, readdata);
    // assert(!status);

    // error: delete non-existent file
    status = fs_delete("user1", "/dir/foo");
    // assert(!status);

    // error: delete file w/o user permission
    status = fs_delete("user2", "/dir/file");
    // assert(!status);

    // error: create more than 1 path
    status = fs_create("user1", "/dir/foo1/foo2", 'd');
    // assert(!status);

    // error: delete non-empty directory
    status = fs_create("user1", "/dir/eecs281", 'd');
    // assert(!status);

    status = fs_create("user1", "/dir/eecs281/midterm", 'f');
    // assert(!status);

    status = fs_writeblock("user1", "/dir/eecs281/midterm", 0, writedata);
    // assert(!status);

    status = fs_delete("user1", "/dir/eecs281");
    // assert(!status);
}

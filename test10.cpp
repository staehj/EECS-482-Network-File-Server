#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

using std::cout;

int main(int argc, char *argv[]) {
    char *server;
    int server_port;

    const char *writedata = "In this project, you will implement a multi-threaded network file server. Clients that use your file server will interact with it via network messages. This project will help you understand hierarchical file systems, socket programming, client-server systems, and network protocols, and it will give you experience building a substantial multi-threaded program with fine-grained locking.";
    const char *writedata1 = "In this project, you will implement a multi-threaded network file server.";
    const char *writedata2 = "In this project, you will implement a multi-threaded network file server.";
    const char *writedata3 = "In this project, you will implement a multi-threaded network file server.";
    const char *writedata4 = "In this project, you will implement a multi-threaded network file server.";
    const char *writedata5 = "In this project, you will implement a multi-threaded network file server.";
    const char *writedata6 = "In this project, you will implement a multi-threaded network file server.";
    const char *writedata7 = "In this project, you will implement a multi-threaded network file server.";
    const char *writedata8 = "In this project, you will implement a multi-threaded network file server.";
    const char *writedata9 = "In this project, you will implement a multi-threaded network file server.";
    const char *writedata10 = "In this project, you will implement a multi-threaded network file server.";
    const char *writedata11 = "In this project, you will implement a multi-threaded network file server.";
    const char *writedata12 = "In this project, you will implement a multi-threaded network file server.";


    char readdata[FS_BLOCKSIZE];
    int status;

    if (argc != 3) {
        cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    fs_delete("user1", "/dir");
    // assert(!status);

    fs_delete("user2", "/dir/foo/bar");
    // assert(!status);

    fs_delete("user3", "/dir/foo");
    // assert(!status);

    fs_create("user1", "/dir", 'd');

    fs_create("user1", "/dir/dir1", 'd');

    fs_create("user1", "/dir/dir1/dir2", 'd');

    fs_create("user1", "/dir/dir1/dir2", 'd');

    fs_writeblock("user1", "/dir/dir1/dir2", 0, writedata);

    fs_writeblock("user1", "/dir/dir1/dir2", 2, writedata);

    fs_create("user1", "/dir/dir1/dir2/file", 'f');

    fs_writeblock("user1", "/dir/dir1/dir2/file", 3, writedata);

    fs_writeblock("user1", "/dir/dir1/dir2/file", 0, writedata1);

    fs_writeblock("user1", "/dir/dir1/dir2/file", 1, writedata1);

    fs_readblock("user2", "/dir/dir1/dir2/file", 0, readdata);

    fs_writeblock("user1", "/dir/dir1/dir2/file", 0, writedata2);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 0, writedata3);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 0, writedata4);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 0, writedata5);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 0, writedata6);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 0, writedata7);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 0, writedata8);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 0, writedata9);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 0, writedata10);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 0, writedata11);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 0, writedata12);

    fs_writeblock("user1", "/dir/dir1/dir2/file", 1, writedata2);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 1, writedata3);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 1, writedata4);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 1, writedata5);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 1, writedata6);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 1, writedata7);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 1, writedata8);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 1, writedata9);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 1, writedata10);
    fs_writeblock("user1", "/dir/dir1/dir2/file", 1, writedata11);
    fs_delete("user1", "/dir/dir1/dir2/file");
    fs_writeblock("user1", "/dir/dir1/dir2/file", 1, writedata12);




    
}


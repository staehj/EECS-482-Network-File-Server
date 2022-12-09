#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

using std::cout;

int main(int argc, char *argv[]) {
    char *server;
    int server_port;

    const char *writedata_contains_null = "In this project, you will implement a multi-threaded network file server.\0 Clients that use your file server will interact with it via network messages.\0 This project will help you understand hierarchical file systems, socket programming, client-server systems, and network protocols, and it will give you experience building a substantial multi-threaded program with fine-grained locking\0. Since clients are untrusted, your file server should be careful in how it handles network input.\0 Avoid making assumptions about the content and size of the requests until you have verified those assumptions.";
    const char *writedata_empty = "";

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

    // error: root directory delete request
    status = fs_delete("user1", "/");
    // assert(!status);

    status = fs_create("user1", "/dir/eecs482", 'd');
    // assert(!status);

    status = fs_create("user1", "/dir/eecs482/project4", 'd');
    // assert(!status);

    status = fs_create("user1", "/dir/eecs482/project4/spec", 'f');
    // assert(!status);

    // error: write data block out of range
    status = fs_writeblock("user1", "/dir/eecs482/project4/spec", 1, writedata_contains_null);
    // assert(!status);

    // write data contains null
    status = fs_writeblock("user1", "/dir/eecs482/project4/spec", 0, writedata_contains_null);
    // assert(!status);

    // write data contains null
    status = fs_writeblock("user1", "/dir/eecs482/project4/spec", 1, writedata_contains_null);
    // assert(!status);

    // read data contains null
    status = fs_readblock("user1", "/dir/eecs482/project4/spec", 0, readdata);
    // assert(!status);

    // error: read request block out of range
    status = fs_readblock("user1", "/dir/eecs482/project4/spec", 2, readdata);
    // assert(!status);

    status = fs_delete("user1", "/dir/eecs482/project4/spec");
    // assert(!status);

    // error: read request to deleted file
    status = fs_readblock("user1", "/dir/eecs482/project4/spec", 1, readdata);
    // assert(!status);

    // error: write request to deleted file
    status = fs_writeblock("user1", "/dir/eecs482/project4/spec", 0, writedata_contains_null);
    // assert(!status);

    status = fs_create("user1", "/dir/eecs482/project4/lab", 'd');
    // assert(!status);

    status = fs_create("user1", "/dir/eecs482/project4/lab/notes", 'f');
    // assert(!status);

    // error: write data empty -> will loop forever
    status = fs_writeblock("user1", "/dir/eecs482/project4/lab/notes", 0, writedata_empty);
    // assert(!status);

    status = fs_readblock("user1", "/dir/eecs482/project4/lab/notes", 0, readdata);
    // assert(!status);



}

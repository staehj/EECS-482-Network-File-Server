#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

using std::cout;

// jjamppong

int main(int argc, char* argv[]) {
    char* server;
    int server_port;

    const char* writedata = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

    char readdata[FS_BLOCKSIZE];
    int status;

    if (argc != 3) {
        cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    status = fs_create("user1", "/file_r1", 'f');

    status = fs_create("user1", "/file_r2", 'f');

    status = fs_writeblock("user1", "/file_r1", 0, writedata);

    status = fs_writeblock("user2", "/file_r1", 0, writedata);

    status = fs_writeblock("user1", "/file_r1", 0, writedata);

    status = fs_writeblock("user1", "/file_r1", 1, writedata);
    
    status = fs_writeblock("user1", "/file_r1", 2, writedata);
    
    status = fs_readblock("user2", "/dir/file", 0, readdata); // ERROR
    
    // status = fs_readblock("user2", "/dir/file", 1, readdata);

    // status = fs_readblock("user1", "/dir/file", 3, readdata);

    // status = fs_readblock("user1", "/dir/file", 2, readdata);

    // status = fs_readblock("user1", "/dir/file", 1, readdata);

    // status = fs_readblock("user1", "/dir/file", 0, readdata);

    // status = fs_delete("user1", "/file_r1");

    // status = fs_delete("user1", "/file_r2");

    // //

    // status = fs_create("user1", "/dir", 'd');

    // status = fs_create("user1", "/dir/dir1-1", 'd');

    // status = fs_create("user1", "/dir/file1-1", 'f');

    // status = fs_create("user1", "/dir/file1-2", 'f');

    // status = fs_create("user1", "/dir/dir1-2", 'd');

    // status = fs_create("user1", "/dir/dir1-3", 'd');

    // status = fs_create("user1", "/dir/dir1-4", 'd');

    // status = fs_create("user1", "/dir/dir1-5", 'd');

    // status = fs_create("user1", "/dir/dir1-6", 'd');

    // status = fs_create("user1", "/dir/dir1-7", 'd');

    // status = fs_writeblock("user1", "/dir/file1-1", 0, writedata);

    // status = fs_writeblock("user1", "/dir/file1-1", 1, writedata);

    // status = fs_writeblock("user1", "/dir/file1-1", 2, writedata);

    // status = fs_writeblock("user1", "/dir/file1-1", 1, writedata);

    // status = fs_writeblock("user1", "/dir/file1-1", 0, writedata);

    // status = fs_delete("user1", "/dir/file1-1");

    // status = fs_delete("user1", "/dir/file1-2");
}

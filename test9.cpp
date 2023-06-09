
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <string>

#include "fs_client.h"

using std::cout;

// make 9 directories and delete 1

int main(int argc, char* argv[]) {
    char* server;
    int server_port;

    const char* writedata1 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    // const char* writedata2 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

    char readdata[FS_BLOCKSIZE];
    int status;

    if (argc != 3) {
        cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    fs_create("user1", "/file", 'f');

    for (int i = 0; i < 123; ++i) {
        fs_writeblock("user1", "/file", i, writedata1);
    }

    fs_readblock("user1", "/file", 123, readdata);

    fs_writeblock("user1", "/file", 123, writedata1);
    fs_writeblock("user1", "/file", 124, writedata1);
    fs_writeblock("user1", "/file", 125, writedata1);
    fs_writeblock("user1", "/file", 60, writedata1);
    fs_writeblock("user1", "/file", 0, writedata1);
    fs_writeblock("user1", "/file", -1, writedata1);
    fs_writeblock("user1", "/file", 4095, writedata1);

    fs_readblock("user1", "/file", 124, readdata);
    fs_readblock("user1", "/file", 0, readdata);
    fs_readblock("user1", "/file", 123, readdata);
    fs_readblock("user1", "/file", 60, readdata);
    fs_readblock("user1", "/file", -1, readdata);
    fs_readblock("user1", "/file", 4095, readdata);

    fs_delete("user1", "/file");
}

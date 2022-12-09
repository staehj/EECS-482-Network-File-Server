#include <iostream>
#include <cassert>
#include <cstdlib>
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

    fs_create("", "/dir", 'd');
    fs_create("user1", "", 'd');
    fs_create("user1", "/dir", ' ');
    fs_create("", "", ' ');

    fs_create("user1", "/dir", 'd');
    fs_create("user1", "/dir/file", 'f');


    fs_writeblock("", "/dir", 0, writedata1);
    fs_writeblock("user1", "", 0, writedata1);
    fs_writeblock("user1", "/dir", -1, writedata1);
    fs_writeblock("", "", -1, writedata1);

    fs_writeblock("user1", "/dir/file", 0, writedata1);

    fs_readblock("", "/dir", 0, readdata);
    fs_readblock("user1", "", 0, readdata);
    fs_readblock("user1", "/dir", -1, readdata);
    fs_readblock("", "", -1, readdata);

    fs_readblock("user1", "/dir/file", 0, readdata);

    fs_delete("", "/dir");
    fs_delete("user1", "");
    fs_delete("", "");

    fs_delete("user1", "/dir/file");
    fs_delete("user1", "/dir");
}

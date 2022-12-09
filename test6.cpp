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

    fs_create("user1 /dir", "", 'd'); // should this work?

    fs_create("", "user1 /dir", 'd'); // should this work?

    fs_create("user1", "/dir", 'd');

    fs_create("user1", "/dir", 'f');

    fs_delete("user2", "/dir");

    fs_delete("user2", "/");

    fs_create("user1", "/dir/f1", 'f');
    fs_create("user1", "/dir/f2", 'f');
    fs_create("user1", "/dir/f3", 'f');
    fs_create("user1", "/dir/f4", 'f');
    fs_create("user1", "/dir/f5", 'f');
    fs_create("user1", "/dir/f6", 'f');
    fs_create("user1", "/dir/f7", 'f');
    fs_create("user1", "/dir/f8", 'f');

    fs_readblock("user1", "/dir/f1", 0, readdata);

    fs_writeblock("user1", "/dir/f1", 0, writedata1);
    fs_writeblock("user1", "/dir/f1", 1, writedata1);
    fs_writeblock("user1", "/dir/f1", 2, writedata1);

    fs_writeblock("user1", "/dir/f2", 0, writedata1);
    fs_writeblock("user1", "/dir/f3", 0, writedata1);
    fs_writeblock("user1", "/dir/f4", 0, writedata1);

    fs_create("user1", "/dir/f9", 'f');

    fs_delete("user", "/dir/f1");
    fs_delete("user", "/dir/f2");
    fs_delete("user", "/dir/f3");

    fs_create("user1", "/dir/f1", 'd');
    fs_create("user1", "/dir/f1", 'f');
}

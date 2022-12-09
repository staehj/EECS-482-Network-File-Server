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

    fs_create("user1", "/dir", 'd');

    fs_create("user1", "/dir/f1", 'f');
    fs_create("user1", "/dir/f2", 'f');
    fs_create("user1", "/dir/f3", 'f');
    fs_create("user1", "/dir/f4", 'f');
    fs_create("user1", "/dir/f5", 'f');
    fs_create("user1", "/dir/f6", 'f');
    fs_create("user1", "/dir/f7", 'f');
    fs_create("user1", "/dir/f8", 'f');

    fs_create("user1", "/dir/d1", 'd');
    fs_create("user1", "/dir/d2", 'd');
    fs_create("user1", "/dir/d3", 'd');
    fs_create("user1", "/dir/d4", 'd');
    fs_create("user1", "/dir/d5", 'd');
    fs_create("user1", "/dir/d6", 'd');
    fs_create("user1", "/dir/d7", 'd');
    fs_create("user1", "/dir/d8", 'd');

    fs_create("user1", "/dir/a1", 'f');
    fs_create("user1", "/dir/a2", 'd');
    fs_create("user1", "/dir/a3", 'd');
    fs_create("user1", "/dir/a4", 'd');
    fs_create("user1", "/dir/a5", 'f');
    fs_create("user1", "/dir/a6", 'f');
    fs_create("user1", "/dir/a7", 'f');
    fs_create("user1", "/dir/a8", 'd');

    fs_create("user1", "/dir/b1", 'f');
    fs_create("user1", "/dir/b2", 'd');
    fs_create("user1", "/dir/b3", 'd');
    fs_create("user1", "/dir/b4", 'd');
    fs_create("user1", "/dir/b5", 'f');
    fs_create("user1", "/dir/b6", 'f');
    fs_create("user1", "/dir/b7", 'f');
    fs_create("user1", "/dir/b8", 'd');

    fs_delete("user1", "/dir/f1");
    fs_delete("user1", "/dir/d1");
    fs_delete("user1", "/dir/a1");
    fs_delete("user1", "/dir/b1");

    fs_delete("user1", "/dir/f5");
    fs_delete("user1", "/dir/d5");
    fs_delete("user1", "/dir/a5");
    fs_delete("user1", "/dir/b5");

    fs_create("user1", "/dir/x1", 'd');  // 
    // fs_create("user1", "/dir/x2", 'd');
    // fs_create("user1", "/dir/x3", 'd');
    // fs_create("user1", "/dir/x4", 'd');
    // fs_create("user1", "/dir/x5", 'd');
    // fs_create("user1", "/dir/x6", 'd');
    // fs_create("user1", "/dir/x7", 'd');
    // fs_create("user1", "/dir/x8", 'd');
}

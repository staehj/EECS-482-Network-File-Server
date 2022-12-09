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

    status = fs_create("user1", "/dir", 'd');
    assert(!status);;

    status = fs_create("user1", "//dir", 'd');
    assert(!status);;

    status = fs_create("user1", "/dir/", 'd');
    assert(!status);;

    status = fs_create("user1", "/di r", 'd');
    assert(!status);;

    status = fs_create("user1", "/dir/.", 'd');
    assert(!status);

    status = fs_create("user1", "/dir/. /", 'd');
    assert(!status);

    status = fs_create("use r1", "/dir/. /", 'd');
    assert(!status);

    status = fs_create("use r1", "/dir/inner", 'd');
    assert(!status);

    status = fs_create(" user1", "/dir/inner", 'd');
    assert(!status);

    status = fs_create("user1", "/dir/inner", 'd');
    assert(!status);

    status = fs_create("/dir/inner", "/dir/inner", 'd');
    assert(!status);

    status = fs_create("/dir/inner", "/dir2", 'd');
    assert(!status);

    fs_readblock("user1", "/dir", 0, readdata);

    fs_writeblock("user1", "/dir", 0, writedata1);

    fs_create("user1", "/dir/file", 'f');

    fs_create("user1", "/dir/file", 'f');

    fs_delete("user1", "/dir/file");

    fs_readblock("user1", "/dir/file", 0, readdata);

    fs_create("user1", "/dir/file", 'f');

    fs_readblock("user1", "/dir/file", 0, readdata);

    fs_readblock("user1", "/dir/file", 1, readdata);

    fs_readblock("user1", "/dir/file", -1, readdata);

    fs_writeblock("user1", "/dir/file", 1, writedata1);

    fs_writeblock("user1", "/dir/file", 0, writedata1);

    fs_writeblock("user1", "/dir/file", 0.5, writedata1);

    fs_writeblock("user1", "/dir/file", 0, "asdf");

    fs_writeblock("user1", "/dir/file", 1, "asdf");
}

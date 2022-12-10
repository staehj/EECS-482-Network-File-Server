
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

    /////// init code
    // for (int i = 0; i < 124 * 8; ++i) {
    //     fs_create("user1", std::string(std::string("/dir")+std::to_string(i)).c_str(), 'd');
    // }
    ///////

    fs_create("user1", "/file", 'f');
    fs_create("user1", "/bad_dir", 'd');

    fs_delete("user1", "/dir10");
    fs_create("user1", "/file0", 'f');
    fs_create("user1", "/bad_dir", 'd');

    fs_delete("user1", "/dir10");
    fs_delete("user1", "/dir20");

    fs_create("user1", "/dir10/bad_file", 'f');
    fs_create("user1", "/dir10/bad_dir", 'd');
    fs_create("user1", "/dir20/bad_file", 'f');
    fs_create("user1", "/dir10/bad_dir", 'd');

    fs_create("user1", "/dir123/file0", 'f');
    fs_create("user1", "/dir123/dir0", 'd');

    fs_writeblock("user1", "/dir123/file0", 0, writedata1);
    fs_delete("user1", "/dir123");

    fs_delete("user1", "/dir60");
    fs_delete("user1", "/dir70");
    fs_delete("user1", "/dir80");

    fs_writeblock("user1", "/dir123/file0", 0, writedata1);
    fs_writeblock("user1", "/dir123/file0", 1, writedata1);
    fs_writeblock("user1", "/dir123/file0", 2, writedata1);
    fs_writeblock("user1", "/dir123/file0", 3, writedata1);
    fs_writeblock("user1", "/dir123/file0", 4, writedata1);
    fs_writeblock("user1", "/dir123/file0", 4, writedata1);

    fs_delete("user1", "/dir7");
    fs_delete("user1", "/dir8");
    fs_delete("user1", "/dir9");

    fs_create("user1", "/foo1", 'f');
    fs_create("user1", "/foo2", 'f');
    fs_create("user1", "/foo3", 'f');
    fs_create("user1", "/foo4", 'f');
    fs_create("user1", "/foo5", 'f');
    fs_create("user1", "/foo6", 'f');
    fs_create("user1", "/foo7", 'f');
    fs_create("user1", "/foo8", 'f');
    fs_create("user1", "/foo9", 'f');
    fs_create("user1", "/foo10", 'f');
    fs_create("user1", "/foo11", 'f');

    


}

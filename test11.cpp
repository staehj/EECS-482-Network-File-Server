
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

    // ///// init code
    // for (int i = 0; i < 124 * 8; ++i) {
    //     fs_create("user1", std::string(std::string("/file")+std::to_string(i)).c_str(), 'f');
    // }
    // /////

    fs_create("user2", "/failed", 'f');
    fs_create("user2", "/failed", 'd');

    fs_delete("user1", "/file10");
    fs_delete("user1", "/file20");
    fs_delete("user1", "/file30");
    
    fs_create("user2", "/new", 'f');
    fs_create("user2", "/new", 'd');
    fs_create("user2", "/new1", 'f');
    fs_create("user2", "/new2", 'd');
    fs_create("user2", "/new3", 'f');
    fs_create("user2", "/new4", 'd');

    fs_writeblock("user1", "/file7", 0, writedata1);
    fs_writeblock("user1", "/file7", 1, writedata1);
    fs_writeblock("user1", "/file7", 2, writedata1);

    fs_writeblock("user1", "/file77", 0, writedata1);
    fs_writeblock("user1", "/file77", 1, writedata1);

    fs_delete("user1", "/file7");
    fs_delete("user1", "/file77");

    fs_delete("user1", "/new1");
    fs_delete("user1", "/new2");

    fs_create("user1", "/file0", 'f');
    fs_create("user1", "/file34", 'f');

    fs_delete("user1", "/file3");
    fs_delete("user1", "/file11");
    fs_delete("user1", "/file19");
    fs_delete("user1", "/file27");
}


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

    ////// logic for creating full disk
    // for (int i = 0; i < 120*8; ++i) {
    //     fs_create("user1", std::string(std::string("/foo")+std::to_string(i)).c_str(), 'd');
    // }

    // for (int i = 0; i < 120*8; ++i) {
    //     fs_create("user1", std::string(std::string("/foo0/bar")+std::to_string(i)).c_str(), 'd');
    // }

    // for (int i = 0; i < 120*8; ++i) {
    //     fs_create("user1", std::string(std::string("/foo1/hello")+std::to_string(i)).c_str(), 'd');
    // }

    // for (int i = 0; i < 120*8; ++i) {
    //     fs_create("user1", std::string(std::string("/foo2/world")+std::to_string(i)).c_str(), 'd');
    // }
    ////// -------------------------

    for (int i = 0; i < 10; ++i) {
        fs_delete("user1", std::string(std::string("/foo2/world")+std::to_string(i)).c_str());
    }

    fs_create("user1", "/file0", 'f');
    fs_create("user1", "/file1", 'f');
    fs_create("user1", "/file2", 'f');

    fs_writeblock("user1", "./file2", 0, writedata1);
    fs_writeblock("user1", "./file1", 0, writedata1);
    fs_writeblock("user1", "./file2", 1, writedata1);
}

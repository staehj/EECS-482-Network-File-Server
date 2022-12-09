#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

using std::cout;

int main(int argc, char *argv[]) {
    char *server;
    int server_port;

    const char *writedata = "In this project, you will implement a multi-threaded network file server. Clients that use your file server will interact with it via network messages. This project will help you understand hierarchical file systems, socket programming, client-server systems, and network protocols, and it will give you experience building a substantial multi-threaded program with fine-grained locking.";

    char readdata[FS_BLOCKSIZE];
    int status;

    if (argc != 3) {
        cout << "error: usage: " << argv[0] << " <server> <serverPort>\n";
        exit(1);
    }
    server = argv[1];
    server_port = atoi(argv[2]);

    fs_clientinit(server, server_port);

    fs_create("user1", "/eecs280", 'f');
    // assert(!status);

    // error: same user creates dir with name same as existing file
    fs_create("user1", "/eecs280", 'd');
    // assert(!status);

    fs_create("user2", "/eecs281", 'd');
    // assert(!status);

    // error: different user tries to create same file
    fs_create("user2", "/eecs280", 'f');
    // assert(!status);

    fs_create("user3", "/eecs370", 'd');
    // assert(!status);

    // error: different user creating dir with existing name 
    fs_create("user4", "/eecs370", 'd');
    // assert(!status);

    // error: different user creating file with existing dir name 
    fs_create("user4", "/eecs370", 'f');
    // assert(!status);

    fs_delete("user3", "/eecs370");
    // assert(!status);

    // error: re-delete deleted file
    fs_delete("user3", "/eecs370");
    // assert(!status);

    // error: creating file under deleted directory
    fs_create("user3", "/eecs370/finals", 'f');
    // assert(!status);

    fs_create("user2", "/eecs281/lectures", 'f');
    // assert(!status);

    fs_writeblock("user2", "/eecs281/lectures", 0, writedata);
    // assert(!status);

    fs_writeblock("user2", "/eecs281/lectures", 1, writedata);
    // assert(!status);

    fs_delete("user2", "/eecs281/lectures");
    // assert(!status);

    // error: writing to deleted file
    fs_writeblock("user2", "/eecs281/lectures", 2, writedata);
    // assert(!status);

    // re-create deleted file
    fs_create("user2", "/eecs281/lectures", 'f');
    // assert(!status);

    // error: read request to re-created/not-written-to file 
    fs_readblock("user2", "/eecs281/lectures", 0, readdata);
    // assert(!status);

    // error: read request to file not written
    fs_readblock("user1", "/eecs280", 0, readdata);
    // assert(!status);    
}

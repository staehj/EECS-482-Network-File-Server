#include <iostream>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include "fs_client.h"

using std::cout;

struct fs_inode {
    char type;                             // file ('f') or directory ('d')
    char owner[FS_MAXUSERNAME + 1];        // owner of this file or directory
    uint32_t size;                         // size of this file or directory
                                           // in blocks
    uint32_t blocks[FS_MAXFILEBLOCKS];     // array of data blocks for this
                                           // file or directory
};

struct fs_direntry {
    char name[FS_MAXFILENAME + 1];         // name of this file or directory
    uint32_t inode_block;                  // disk block that stores the inode
                                          // for this file or directory (0 if
                                          // this direntry is unused)
};

int main(int argc, char *argv[]) {
    char *server;
    int server_port;

    fs_inode write_data;
    write_data.type = 'd';
    strcpy(write_data.owner, "user1");
    write_data.size = 1;
    write_data.blocks[0] = 1;

    fs_inode write_data2;
    write_data2.type = 'd';
    strcpy(write_data2.owner, "user1");
    write_data2.size = 1;
    write_data2.blocks[0] = 5;

    fs_direntry write_data3 [8];
    strcpy(write_data3[0].name, "ghost");
    write_data3[0].inode_block = 4;


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

    fs_create("user1", "/file/file", 'f');
    fs_create("user1", "/file/dir", 'd');

    fs_delete("user1", "/file/file");
    fs_delete("user1", "/file/dir");

    fs_writeblock("user1", "/file", 0, &write_data);

    fs_create("user1", "/file/file", 'f');
    fs_create("user1", "/file/dir", 'd');

    fs_delete("user1", "/file/file");
    fs_delete("user1", "/file/dir");

    fs_writeblock("user1", "/file", 0, &write_data2);
    fs_readblock("user1", "/file", 0, readdata);

    fs_create("user1", "/file/file", 'f');
    fs_create("user1", "/file/dir", 'd');

    fs_delete("user1", "/file/file");
    fs_delete("user1", "/file/dir");

    fs_writeblock("user1", "/file", 0, write_data3);
    fs_readblock("user1", "/file", 0, readdata);

    fs_delete("user1", "/file/ghost");

    fs_create("user1", "/file/ghost", 'f');
    fs_create("user1", "/file/ghost", 'd');

    fs_delete("user1", "/file/ghost");

    fs_writeblock("user1", "/file/ghost", 0, &write_data);
    fs_readblock("user1", "/file/ghost", 0, readdata);

    fs_delete("user1", "/file/ghost");

    fs_create("user1", "/file", 'd');

    fs_delete("user1", "/file/ghost");

    fs_writeblock("user1", "/file", 0, &write_data);
    fs_writeblock("user1", "/file", 1, &write_data2);
    fs_writeblock("user1", "/file", 2, write_data3);

    fs_delete("user1", "/file/ghost");

    fs_delete("user1", "/file");
}

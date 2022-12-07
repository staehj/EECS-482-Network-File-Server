
#ifndef _FILE_SERVER_H_
#define _FILE_SERVER_H_

#include <queue>
#include <unordered_set>

#include "fs_param.h"
#include "fs_server.h"


class FileServer {
public:
    enum RequestType { READ, WRITE, CREATE, DELETE };

    FileServer(int port_number);


private:
    void handle_request(RequestType type);

    void handle_read();

    void handle_write();

    void handle_create();

    void handle_delete();

    void traverse_fs();


    std::priority_queue<uint32_t, std::vector<uint32_t>, std::greater<uint32_t> > free_blocks;
    fs_inode buf_inode;
    fs_direntry buf_direntries [FS_DIRENTRIES];
    int port_number;
};

#endif /* _FILE_SERVER_H_ */

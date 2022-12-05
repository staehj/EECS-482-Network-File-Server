
#ifndef _FILE_SERVER_H_
#define _FILE_SERVER_H_

#include <queue>
#include <string>
#include <sstream>
#include <unordered_set>

#include "fs_param.h"
#include "fs_server.h"


static const size_t MAX_MESSAGE_SIZE
    = sizeof("FS_WRITEBLOCK") + FS_MAXUSERNAME + FS_MAXPATHNAME
        + sizeof(int) + 4 + FS_BLOCKSIZE;

class Exception {};

class FileServer {
public:
    enum RequestType { READ, WRITE, CREATE, DELETE };

    FileServer(int port_number);

    void thread_start(int connectionfd, int sock);

private:
    int run(int queue_size);

    int handle_message(int connectionfd, int sock);

    RequestType parse(std::string msg);

    std::string check_username(std::stringstream &msg_ss);

    std::string check_pathname(std::stringstream &msg_ss);

    std::string check_block(std::stringstream &msg_ss);

    std::string check_type(std::stringstream &msg_ss);

    void handle_request(RequestType type, std::string request, const char* data);

    void handle_read(std::string request);

    void handle_write(std::string request, const char* data);

    void handle_create(std::string request);

    void handle_delete(std::string request);

    void traverse_fs();

    std::priority_queue<uint32_t, std::vector<uint32_t>, std::greater<uint32_t> > free_blocks;
    fs_inode buf_inode;
    fs_direntry buf_direntries [FS_DIRENTRIES];
    int port;
};

#endif /* _FILE_SERVER_H_ */

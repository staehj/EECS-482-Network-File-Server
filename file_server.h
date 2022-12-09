
#ifndef _FILE_SERVER_H_
#define _FILE_SERVER_H_

#include <queue>
#include <string>
#include <sstream>
#include <unordered_set>
#include <vector>

#include "helpers.h"


class Exception {};

class FileServer {
public:
    class ThreadRAII {
    public:
        ThreadRAII(FileServer* fs, int connectionfd) {
            t = std::thread(&FileServer::thread_start, fs, connectionfd);
        }

        ~ThreadRAII() {
            t.detach();
        }

    private:
        std::thread t;
    };

    enum RequestType { READ, WRITE, CREATE, DELETE };

    FileServer(int port_number);

    void thread_start(int connectionfd);

private:
    int run(int queue_size);

    int handle_message(int connectionfd);

    RequestType parse(std::string msg);

    std::string check_username(std::stringstream &msg_ss);

    std::string check_pathname(std::stringstream &msg_ss);

    std::string check_block(std::stringstream &msg_ss);

    std::string check_type(std::stringstream &msg_ss);

    void handle_request(RequestType type, std::string request, const char* data,
                        int connectionfd);

    void handle_read(std::string request, int connectionfd);

    void handle_write(std::string request, const char* data, int connectionfd);

    void handle_create(std::string request, int connectionfd);

    void handle_delete(std::string request, int connectionfd);

    int find_path(std::deque<std::string> &names, std::string username,
                  std::unique_lock<std::mutex> &cur_lock, fs_inode &cur_inode);

    void decompose_path(std::deque<std::string> &names, std::string pathname);

    void traverse_fs();

    void check_inode_type(fs_inode &cur_inode, char type);
    
    void check_inode_username(fs_inode &cur_inode, std::string username);

    DirEntryIndex check_name_exists_get_free_direntry(fs_inode &inode, std::string name, fs_direntry* buf_direntries);

    void create_inode(fs_inode &cur_inode, int cur_block, std::string username,
                      std::string name, char type, DirEntryIndex direntry_ind, fs_direntry* block_direntries);

    int get_free_block();

    void add_free_block(int block);

    int get_target_inode_block(fs_inode &inode, std::string name,
                               DirEntryIndex &direntry_ind, fs_direntry* buf_direntries);

    void check_directory_empty(fs_inode &inode);

    int get_direntry_count(fs_direntry* buf_direntries);

    void fix_inode(fs_inode &inode, int target);

    std::priority_queue<uint32_t, std::vector<uint32_t>, std::greater<uint32_t> >
        free_blocks;  // TODO?: lock required?
    std::vector<std::mutex> block_locks;
    int port;
};

#endif /* _FILE_SERVER_H_ */

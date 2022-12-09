#include <iostream>
#include <cassert>
#include <cstdlib>
#include "fs_client.h"

using std::cout;

int main(int argc, char *argv[]) {
    char *server;
    int server_port;

    const char *writedata_contains_null = "In this project, you will implement a multi-threaded network file server.\0 Clients that use your file server will interact with it via network messages.\0 This project will help you understand hierarchical file systems, socket programming, client-server systems, and network protocols, and it will give you experience building a substantial multi-threaded program with fine-grained locking\0. Since clients are untrusted, your file server should be careful in how it handles network input.\0 Avoid making assumptions about the content and size of the requests until you have verified those assumptions.";

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
    // assert(!status);

    status = fs_create("user1", "/dir/user", 'd');
    // assert(!status);

    status = fs_create("user1", "/dir/user/Hyunjeong_Sophia_Tae", 'd');
    // assert(!status);

    status = fs_create("user1", "/dir/user/Hyunjeong_Sophia_Tae/documents", 'd');
    // assert(!status);

    status = fs_create("user1", "/dir/user/Hyunjeong_Sophia_Tae/documents/school", 'd');
    // assert(!status);

    status = fs_create("user1", "/dir/user/Hyunjeong_Sophia_Tae/documents/school/University_Of_Michigan", 'd');
    // assert(!status);

    status = fs_create("user1", "/dir/user/Hyunjeong_Sophia_Tae/documents/school/University_Of_Michigan/fall_2020", 'd');
    // assert(!status);

    status = fs_create("user1", "/dir/user/Hyunjeong_Sophia_Tae/documents/school/University_Of_Michigan/fall_2020/eecs", 'd');
    // assert(!status);

    status = fs_create("user1", "/dir/user/Hyunjeong_Sophia_Tae/documents/school/University_Of_Michigan/fall_2020/eecs/eecs281", 'd');
    // assert(!status);

    status = fs_create("user1", "/dir/user/Hyunjeong_Sophia_Tae/documents/school/University_Of_Michigan/fall_2020/eecs/eecs281/midterm", 'd');
    // assert(!status);

    status = fs_create("user1", "/dir/user/Hyunjeong_Sophia_Tae/documents/school/University_Of_Michigan/fall_2020/eecs/eecs281/midterm/exams", 'd');
    // assert(!status);

    status = fs_create("user1", "/dir/user/Hyunjeong_Sophia_Tae/documents/school/University_Of_Michigan/fall_2020/eecs/eecs281/midterm/exams/past_exams", 'd');
    // assert(!status);

    // parsing error: pathname length exceeded
    status = fs_create("user1", "/dir/user/Hyunjeong_Sophia_Tae/documents/school/University_Of_Michigan/fall_2020/eecs/eecs281/midterm/exams/past_exams/past_exams_solutions", 'd');
    // assert(!status);

    // pathname length == FS_MAXPATHNAME 
    status = fs_create("user1", "/dir/user/Hyunjeong_Sophia_Tae/documents/school/University_Of_Michigan/fall_2020/eecs/eecs281/midterm/exams/past_exams/solutions", 'd');
    // assert(!status);

    // parsing error: filename length exceeded
    status = fs_create("user1", "/dir/eecs482", 'd');
    // assert(!status);
    status = fs_create("user1", "/dir/eecs482/Hyunjeong_Sophia_Tae_Ohjun_Kweon_Dong_Sub_Kim_Brian_Noble_Every_One_Else_In_EECS_482", 'f');
    // assert(!status);

}

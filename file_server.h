
#ifndef _FILE_SERVER_H_
#define _FILE_SERVER_H_


class FileServer {
public:
    enum RequestType { READ, WRITE, CREATE, DELETE };

    FileServer(int port_number);


private:
    void run();

    void handle_request(RequestType type);

    void handle_read();

    void handle_write() ;

    void handle_create();

    void handle_delete();


    int port_number;
};

#endif /* _FILE_SERVER_H_ */

#include "file_server.h"

#include "exception.h"

FileServer::FileServer(int port_number) {
    port_number = port_number;

    // TODO: do other stuff from spec
}

void FileServer::run() {
    
}

void FileServer::handle_request(RequestType type) {
    // TODO maybe some RAII stuff for exception/error handling
    switch(type) {
        case RequestType::READ:
            handle_read();
            break;
        case RequestType::WRITE:
            handle_write();
            break;
        case RequestType::CREATE:
            handle_create();
            break;
        case RequestType::DELETE:
            handle_delete();
            break;
        default:
            // TODO: error stuff
    }
}

void FileServer::handle_read() {

}

void FileServer::handle_write() {

}

void FileServer::handle_create() {

}

void FileServer::handle_delete() {

}

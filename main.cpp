#include <iostream>
#include <cassert>

#include "file_server.h"


int main(int argc, char *argv[]) {
    int port_number;
    if (argc == 1) {
        port_number = 0; // let OS decide
    }
    else if (argc == 2) {
        port_number = atoi(argv[1]);
    }
    else {
        exit(1);
    }

    // Initialize FileServer
    FileServer fs(port_number);
}

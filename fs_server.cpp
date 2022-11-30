#include <iostream>
#include <cassert>

#include "file_server.h"


int main(int argc, char *argv[]) {
    int port_number;
    if (argc == 2) {
        port_number = // let OS decide
    }
    else if (argc == 3) {
        port_number = atoi(argv[2]);
    }
    else {
        // error, there should be either 1 or 2 args
    }

    // Initialize FileServer
    FileServer fs(port_number);

    // Broadcast FileServer boot
    std::cout << "\n@@@ port " << port_number << std::endl;

    // Start accepting requests
    fs.run();
}

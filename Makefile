<<<<<<< HEAD
CC=g++ -g -Wall -std=c++17
=======
ifeq (${shell uname},Darwin)
    CC=g++ -g -Wall -std=c++17 -D_XOPEN_SOURCE
    LIBFS_SERVER=libfs_server_macos.o
    LIBFS_CLIENT=libfs_client_macos.o
else
    CC=g++ -g -Wall -std=c++17
    LIBFS_SERVER=libfs_server.o
    LIBFS_CLIENT=libfs_client.o
endif
>>>>>>> 87f82768a9059a3ae48a8776ee0c5bb47a1b3912

# List of source files for your file server
FS_SOURCES=file1.cpp file2.cpp

# Generate the names of the file server's object files
FS_OBJS=${FS_SOURCES:.cpp=.o}

all: fs app

# Compile the file server and tag this compilation
<<<<<<< HEAD
fs: ${FS_OBJS} libfs_server.o
	./autotag.sh
	${CC} -o $@ $^ -pthread -ldl

# Compile a client program
app: app.cpp libfs_client.o
	${CC} -o $@ $^

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${FS_OBJS} fs app
=======
fs: ${FS_OBJS} ${LIBFS_SERVER}
    ./autotag.sh
    ${CC} -o $@ $^ -pthread -ldl

# Compile a client program
app: app.cpp ${LIBFS_CLIENT}
    ${CC} -o $@ $^

# Generic rules for compiling a source file to an object file
%.o: %.cpp
    ${CC} -c $<
%.o: %.cc
    ${CC} -c $<

clean:
    rm -f ${FS_OBJS} fs app
>>>>>>> 87f82768a9059a3ae48a8776ee0c5bb47a1b3912

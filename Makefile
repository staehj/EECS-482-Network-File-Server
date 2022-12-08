ifeq (${shell uname},Darwin)
	CC=g++ -g -Wall -std=c++17 -D_XOPEN_SOURCE
	LIBFS_SERVER=libfs_server_macos.o
	LIBFS_CLIENT=libfs_client_macos.o
else
	CC=g++ -g -Wall -std=c++17
	LIBFS_SERVER=libfs_server.o
	LIBFS_CLIENT=libfs_client.o
endif

# List of source files for your file server
FS_SOURCES=main.cpp file_server.cpp helpers.cpp

# Generate the names of the file server's object files
FS_OBJS=${FS_SOURCES:.cpp=.o}

all: fs app

# Compile the file server and tag this compilation
fs: ${FS_OBJS} ${LIBFS_SERVER}
	./createfs_macos
	./autotag.sh
	${CC} -o $@ $^ -pthread -ldl

# Compile a client program
app: test_spec.cpp ${LIBFS_CLIENT}
	${CC} -o $@ $^

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${FS_OBJS} fs app
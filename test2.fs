/ (type d) (inode block 0)
	owner: 
	size: 1
	data disk blocks: 1 
	entry 0: dir, inode block 2

/dir (type d) (inode block 2)
	owner: user1
	size: 1
	data disk blocks: 3 
	entry 0: file, inode block 4
	entry 1: eecs281, inode block 7

/dir/file (type f) (inode block 4)
	owner: user1
	size: 2
	data disk blocks: 5 6 
In this project, you will implement a multi-threaded network file server. Clients that use your file server will interact with it via network messages. This project will help you understand hierarchical file systems, socket programming, client-server systems, and network protocols, and it will give you experience building a substantial multi-threaded program with fine-grained locking. error: usage:   <server> <serverPort>
 user1 /dir /dir/file/ dir/file user1user1user1user1user1user1 /dir/file user2 /dir/foIn this project, you will implement a multi-threaded network file server. Clients that use your file server will interact with it via network messages. This project will help you understand hierarchical file systems, socket programming, client-server systems, and network protocols, and it will give you experience building a substantial multi-threaded program with fine-grained locking. error: usage:   <server> <serverPort>
 user1 /dir /dir/file/ dir/file user1user1user1user1user1user1 /dir/file user2 /dir/fo

/dir/eecs281 (type d) (inode block 7)
	owner: user1
	size: 1
	data disk blocks: 8 
	entry 0: midterm, inode block 9

/dir/eecs281/midterm (type f) (inode block 9)
	owner: user1
	size: 1
	data disk blocks: 10 
In this project, you will implement a multi-threaded network file server. Clients that use your file server will interact with it via network messages. This project will help you understand hierarchical file systems, socket programming, client-server systems, and network protocols, and it will give you experience building a substantial multi-threaded program with fine-grained locking. error: usage:   <server> <serverPort>
 user1 /dir /dir/file/ dir/file user1user1user1user1user1user1 /dir/file user2 /dir/fo

4085 disk blocks free

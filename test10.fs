/ (type d) (inode block 0)
	owner: 
	size: 1
	data disk blocks: 1 
	entry 0: dir, inode block 2

/dir (type d) (inode block 2)
	owner: user1
	size: 1
	data disk blocks: 3 
	entry 0: dir1, inode block 4

/dir/dir1 (type d) (inode block 4)
	owner: user1
	size: 1
	data disk blocks: 5 
	entry 0: dir2, inode block 6

/dir/dir1/dir2 (type d) (inode block 6)
	owner: user1
	size: 0
	data disk blocks: 

4089 disk blocks free

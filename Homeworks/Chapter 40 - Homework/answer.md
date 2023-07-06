1. Run the simulator with some different random seeds (say 17, 18, 19, 20), and see if you can figure out which operations must have taken place between each state change.
Answer:
Initial state

-s 1
inode bitmap  10000000
inodes       [d a:0 r:2][][][][][][][]
data bitmap   10000000
data         [(.,0) (..,0)][][][][][][][]

Which operation took place?
inode bitmap  11000000
inodes       [d a:0 r:3][d a:1 r:2][][][][][][]
data bitmap   11000000
data         [(.,0) (..,0) (n,1)][(.,1) (..,0)][][][][][][]
Operation: mkdir("/n")

Which operation took place?
inode bitmap  11100000
inodes       [d a:0 r:3][d a:1 r:2][f a:-1 r:1][][][][][]
data bitmap   11000000
data         [(.,0) (..,0) (n,1) (w,2)][(.,1) (..,0)][][][][][][]
Operation: creat("w")

Which operation took place?
inode bitmap  11000000
inodes       [d a:0 r:3][d a:1 r:2][][][][][][]
data bitmap   11000000
data         [(.,0) (..,0) (n,1)][(.,1) (..,0)][][][][][][]
Operation: unlink("w")

Which operation took place?
inode bitmap  11100000
inodes       [d a:0 r:3][d a:1 r:2][f a:-1 r:1][][][][][]
data bitmap   11000000
data         [(.,0) (..,0) (n,1)][(.,1) (..,0) (x,2)][][][][][][]
Operation: creat(/n/x)

Which operation took place?
inode bitmap  11100000
inodes       [d a:0 r:3][d a:1 r:2][f a:2 r:1][][][][][]
data bitmap   11100000
data         [(.,0) (..,0) (n,1)][(.,1) (..,0) (x,2)][o][][][][][]
Operation: fd=open("/n/x", O_WRONLY|O_APPEND); write(fd, buf, BLOCKSIZE); close(fd);

Which operation took place?
inode bitmap  11110000
inodes       [d a:0 r:3][d a:1 r:2][f a:2 r:1][f a:-1 r:1][][][][]
data bitmap   11100000
data         [(.,0) (..,0) (n,1) (m,3)][(.,1) (..,0) (x,2)][o][][][][][]
Operation: creat("/m")

Which operation took place?
inode bitmap  11010000
inodes       [d a:0 r:3][d a:1 r:2][][f a:-1 r:1][][][][]
data bitmap   11000000
data         [(.,0) (..,0) (n,1) (m,3)][(.,1) (..,0)][][][][][][]
Operation: unlink("/n/x")

Which operation took place?
inode bitmap  11010000
inodes       [d a:0 r:3][d a:1 r:2][][f a:2 r:1][][][][]
data bitmap   11100000
data         [(.,0) (..,0) (n,1) (m,3)][(.,1) (..,0)][l][][][][][]
Operation: fd=open("/m", O_WRONLY|O_APPEND); write(fd, buf, BLOCKSIZE); close(fd);

Which operation took place?
inode bitmap  11010000
inodes       [d a:0 r:3][d a:1 r:2][][f a:2 r:2][][][][]
data bitmap   11100000
data         [(.,0) (..,0) (n,1) (m,3)][(.,1) (..,0) (e,3)][l][][][][][]
Operation: link("/m", "/n/e")

Which operation took place?
inode bitmap  11110000
inodes       [d a:0 r:3][d a:1 r:2][f a:-1 r:1][f a:2 r:2][][][][]
data bitmap   11100000
data         [(.,0) (..,0) (n,1) (m,3) (t,2)][(.,1) (..,0) (e,3)][l][][][][][]
Operation: creat("/t")

-s 0
Initial state
inode bitmap  10000000
inodes       [d a:0 r:2][][][][][][][]
data bitmap   10000000
data         [(.,0) (..,0)][][][][][][][]

Which operation took place?
inode bitmap  11000000
inodes       [d a:0 r:3][d a:1 r:2][][][][][][]
data bitmap   11000000
data         [(.,0) (..,0) (g,1)][(.,1) (..,0)][][][][][][]
Operation: mkdir("/g")

Which operation took place?
inode bitmap  11100000
inodes       [d a:0 r:3][d a:1 r:2][f a:-1 r:1][][][][][]
data bitmap   11000000
data         [(.,0) (..,0) (g,1) (q,2)][(.,1) (..,0)][][][][][][]
Operation: creat("/q")

Which operation took place?
inode bitmap  11110000
inodes       [d a:0 r:3][d a:1 r:2][f a:-1 r:1][f a:-1 r:1][][][][]
data bitmap   11000000
data         [(.,0) (..,0) (g,1) (q,2) (u,3)][(.,1) (..,0)][][][][][][]
Operation: creat("/u")

Which operation took place?
inode bitmap  11110000
inodes       [d a:0 r:3][d a:1 r:2][f a:-1 r:1][f a:-1 r:2][][][][]
data bitmap   11000000
data         [(.,0) (..,0) (g,1) (q,2) (u,3) (x,3)][(.,1) (..,0)][][][][][][]
Operation: link("/u", "/x")

2. Now do the same, using different random seeds (say 21, 22, 23, 24), except run with the -r flag, thus making you guess the state change while being shown the operation. What can you conclude about the inode and data-block allocation algorithms, in terms of which blocks they prefer to allocate?
Answer:

Initial state

inode bitmap  10000000
inodes       [d a:0 r:2][][][][][][][]
data bitmap   10000000
data         [(.,0) (..,0)][][][][][][][]

creat("/d");
  State of file system:
  inode bitmap  11000000
  inodes       [d a:0 r:2][f a:-1 r:1][][][][][][]
  data bitmap   10000000
  data         [(.,0) (..,0) (d,1)][][][][][][][]

fd=open("/d", O_WRONLY|O_APPEND); write(fd, buf, BLOCKSIZE); close(fd);
  State of file system:
  inode bitmap  11000000
  inodes       [d a:0 r:2][f a:1 r:1][][][][][][]
  data bitmap   11000000
  data         [(.,0) (..,0) (d,1)][d][][][][][][]

link("/d", "/h");
  State of file system:
  inode bitmap  11000000
  inodes       [d a:0 r:2][f a:1 r:2][][][][][][]
  data bitmap   11000000
  data         [(.,0) (..,0) (d,1) (h,1)][d][][][][][][]

creat("/g");
  State of file system:
  inode bitmap  11100000
  inodes       [d a:0 r:2][f a:1 r:2][f a:-1 r:1][][][][][]
  data bitmap   11000000
  data         [(.,0) (..,0) (d,1) (h,1) (g,2)][d][][][][][][]

The algorithm look for the first available block at bitmap.

3. Now reduce the number of data blocks in the file system, to very low numbers (say two), and run the simulator for a hundred or so requests. What types of files end up in the file system in this highlyconstrained layout? What types of operations would fail?
Answer: Just Files types. Creating directory and writing to files.


4. Now do the same, but with inodes. With very few inodes, what types of operations can succeed? Which will usually fail? What is the final state of the file system likely to be?
Answer: Link, unlink and write operations. creat and mkdir will usually fail. Few inodes and high number of files linked to the same inode.
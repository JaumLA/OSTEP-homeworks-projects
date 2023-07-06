1. First, run fsck.py -D; this flag turns off any corruption, and thus you can use it to generate a random file system, and see if you can determine which files and directories are in there. So, go ahead and do that! Use the -p flag to see if you were right. Try this for a few different randomly-generated file systems by setting the seed (-s) to different values, like 1, 2, and 3.
Answer: Yes.


2. Now, let’s introduce a corruption. Run fsck.py -S 1 to start. Can you see what inconsistency is introduced? How would you fix it in a real file system repair tool? Use -c to check if you were right.
Answer:
Final state of file system with -S 1:
  inode bitmap 1000100010000001
  inodes       [d a:0 r:4] [] [] [] [d a:12 r:2] [] [] [] [d a:6 r:2] [] [] [] [] [f a:-1 r:2] [] [f a:-1 r:1] 
  data bitmap  1000001000001000
  data         [(.,0) (..,0) (g,8) (w,4) (m,13) (z,13)] [] [] [] [] [] [(.,8) (..,0) (s,15)] [] [] [] [] [] [(.,4) (..,0)] [] [] [] 

Can you figure out how the file system was corrupted?
  A: Corruption: inode bitmap corrupt bit 13. To fix this we can check for consistency at inode bitmap and allocated inodes.

Final state of file system with -S 2:
  inode bitmap 1000100010000101
  inodes       [d a:0 r:4] [] [] [] [d a:12 r:2] [] [] [] [f a:6 r:2] [] [] [] [] [f a:-1 r:2] [] [f a:-1 r:1] 
  data bitmap  1000001000001000
  data         [(.,0) (..,0) (g,8) (w,4) (m,13) (z,13)] [] [] [] [] [] [(.,8) (..,0) (s,15)] [] [] [] [] [] [(.,4) (..,0)] [] [] [] 

Can you figure out how the file system was corrupted?
  A: Wrong type at inode bitmap 8, it was file now is dir. To fix this we can compare the file type at the inode and the actual data block.

3. Change the seed to -S 3 or -S 19; which inconsistency do you see? Use -c to check your answer. What is different in these two cases?
Answer:
  For -S 3 there is a corruption at inode 15, there was two references to file "g/s" now there is only one but the r count still 2.
  For -S 19 there is a corruption at the number of references in inode 8, the directory has only one reference, but there is two data references to it , the reference at the current folder and itself.

4. Change the seed to -S 5; which inconsistency do you see? How hard would it be to fix this problem in an automatic way? Use -c to check your answer. Then, introduce a similar inconsistency with -S 38; is this harder/possible to detect? Finally, use -S 642; is this inconsistency detectable? If so, how would you fix the file system?
Answer:
  For -S 5 there is a corruption at inode 8 the name of entry ('s', 15) altered to refer to different name (y). To automatic fix this, we need some type of logging to keep track of updates.

  For -S 38 the inconsistency is easier to identify, the data at 12 has the wrong name of the previous folder.

  For -S 642 the incosistency is at the inode 0, there are two directories with the same name. Check the current files names.

5. Change the seed to -S 6 or -S 13; which inconsistency do you see? Use -c to check your answer. What is the difference across these two cases? What should the repair tool do when encountering such a situation?
Answer:
  With -S 6 there is garbage at inode 12.
  With -S 13 there is garbage at inode 10.
  The difference between the two is that for -S 6 the inode points to a directory that not exists no more, the -S 13 was to a file.
  Check if the inode bitmap is allocated at the current inode.


6. Change the seed to -S 9; which inconsistency do you see? Use -c to check your answer. Which piece of information should a checkand-repair tool trust in this case? 
Answer: Inode 13 was type file, now dir. 


7. Change the seed to -S 15; which inconsistency do you see? Use -c to check your answer. What can a repair tool do in this case? If no repair is possible, how much data is lost?
Answer: Inode 0 was dir, now it's file type.


8. Change the seed to -S 10; which inconsistency do you see? Use -c to check your answer. Is there redundancy in the file system structure here that can help a repair?
Answer: Corruption at inode 4, the data block (.., 3) points to an free inode space. Yes, the inode that was to be allocated.


9. Change the seed to -S 16 and -S 20; which inconsistency do you see? Use -c to check your answer. How should the repair tool fix the problem?
Answer:
  For -S 16 there is a corruption at inode 13, the address of the data block is not allocated.
  For -S 20 there is a corruption at inode 8, the address of the data block is not allocated.
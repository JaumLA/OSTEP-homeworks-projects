1. Run ./lfs.py -n 3, perhaps varying the seed (-s). Can you figure out which commands were run to generate the final file system contents? Can you tell which order those commands were issued? Finally, can you determine the liveness of each block in the final file system state? Use -o to show which commands were run, and -c to show the liveness of the final file system state. How much harder does the task become for you as you increase the number of commands issued (i.e., change -n 3 to -n 5)?
Answer:
 With -n 3 -s 1 the commands are 
  create file /tg4
  write file /tg4
  create file /lt0
  The block alive are: 0, 8, 10, 11, 12, 13

 With -n 3 -s 2 the commands are
  create file /vt6
  write file /vt6, offset 4, size 4 
  create file /ks9
  The live blocks are: 0, 8, 9, 10, 11, 12, 14, 15, 16, 17

 With -n 5 -s 1 the commands are
  create file /tf4
  write /tf4, offset 6, size 0
  create /lt0
  write /lt0, offset 1, size 7
  link /oy3, /tg4
  The live blocks are: 0, 14, 15, 16, 17, 18, 19, 20, 23, 24, 25, 26

2. If you find the above painful, you can help yourself a little bit by showing the set of updates caused by each specific command. To do so, run ./lfs.py -n 3 -i. Now see if it is easier to understand what each command must have been. Change the random seed to get different commands to interpret (e.g., -s 1, -s 2, -s 3, etc.).
Answer: It's easier.


3. To further test your ability to figure out what updates are made to disk by each command, run the following: ./lfs.py -o -F -s 100 (and perhaps a few other random seeds). This just shows a set of commands and does NOT show you the final state of the file system. Can you reason about what the final state of the file system must be?
Answer:
  With -F -s 100
    create file /us7
      [0] live checkpoint: 7
      ...
      [4] live [.,0] [..,0] [us7, 1] -- -- -- -- --
      [5] live type:dir size:1 refs:2 ptrs: 4 -- -- -- -- -- -- -- 
      [6] live type:reg size:0 refs:1 ptrs: -- -- -- -- -- -- -- --
      [7] live chunk(imap): 5 6 -- -- -- -- -- -- -- -- -- -- -- -- -- --
    
    write file /us7 offset=4 size=0
      [0] live checkpoint: 9
      ...
      [8] live type:reg size:4 refs:1 ptrs: -- -- -- -- -- -- -- --
      [9] live chunk(imap): 5 8 -- -- -- -- -- -- -- -- -- -- -- -- -- --

    write file /us7 offset=7 size=7
      [0] live checkpoint 12
      ...
      [10] live file data
      [11] live type:reg size:8 refs:1 ptrs: -- -- -- -- -- -- -- 10
      [12] live chunk(imap): 5 11

    The final state is:
      [0 ] live checkpoint: 7
      [1 ]      [.,0] [..,0] -- -- -- -- -- -- 
      [2 ]      type:dir size:1 refs:2 ptrs: 1 -- -- -- -- -- -- -- 
      [3 ]      chunk(imap): 2 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
      [4 ] live [.,0] [..,0] [us7, 1] -- -- -- -- --
      [5 ] live type:dir size:1 refs:2 ptrs: 4 -- -- -- -- -- -- -- 
      [6 ]      type:reg size:0 refs:1 ptrs: -- -- -- -- -- -- -- --
      [7 ]      chunk(imap): 5 6 -- -- -- -- -- -- -- -- -- -- -- -- -- --
      [8 ]      type:reg size:4 refs:1 ptrs: -- -- -- -- -- -- -- --
      [9 ]      chunk(imap): 5 8 -- -- -- -- -- -- -- -- -- -- -- -- -- --
      [10] live file data
      [11] live type:reg size:8 refs:1 ptrs: -- -- -- -- -- -- -- 10
      [12] live chunk(imap): 5 11


4. Now see if you can determine which files and directories are live after a number of file and directory operations. Run tt ./lfs.py -n 20 -s 1 and then examine the final file system state. Can you figure out which pathnames are valid? Run tt ./lfs.py -n 20 -s 1 -c -v to see the results. Run with -o to see if your answers match up given the series of random commands. Use different random seeds to get more problems.
Answer:
  With -n 20 -s 1: 
    The live blocks: 0, 34, 60, 64, 65, 66, 68, 69, 70, 76, 77, 78, 79, 80, 91, 93, 94, 95, 97, 98, 99.
    Valid pathnames: /, /ln7, /lt0, /af4, /ln7/zp3, /ln7/zu5


5. Now let’s issue some specific commands. First, let’s create a file and write to it repeatedly. To do so, use the -L flag, which lets you specify specific commands to execute. Let’s create the file ”/foo” and write to it four times: -L c,/foo:w,/foo,0,1:w,/foo,1,1:w,/foo,2,1:w,/foo,3,1 -o. See if you can determine the liveness of the final file system state; use -c to check your answers.
Answer: Live blocks: 0, 4, 5, 8, 11, 14, 17, 18, 19


6. Now, let’s do the same thing, but with a single write operation instead of four. Run ./lfs.py -o -L c,/foo:w,/foo,0,4 to create file ”/foo” and write 4 blocks with a single write operation. Compute the liveness again, and check if you are right with -c. What is the main difference between writing a file all at once (as we do here) versus doing it one block at a time (as above)? What does this tell you about the importance of buffering updates in main memory as the real LFS does?
Answer: Live blocks: 0, 4, 5, 8, 9, 10, 11, 12, 13. The file's contents are sequentialy allocated, instead of scattered between blocks.


7. Let’s do another specific example. First, run the following: ./lfs.py -L c,/foo:w,/foo,0,1. What does this set of commands do? Now, run ./lfs.py -L c,/foo:w,/foo,7,1. What does this set of commands do? How are the two different? What can you tell about the size field in the inode from these two sets of commands? 
Answer: This set of commands ./lfs.py -L c,/foo:w,/foo,0,1 make a file and write to it.
  The set of commands ./lfs.py -L c,/foo:w,/foo,7,1 make a file and write to it with offset=7.
  The difference is just the location of the data block pointer.


8. Now let’s look explicitly at file creation versus directory creation. Run simulations ./lfs.py -L c,/foo and ./lfs.py -L d,/foo to create a file and then a directory. What is similar about these runs, and what is different?
Answer: The similarity is that we need to update the block that they were created and update the imap. The difference is that we allocate a block for an empty directory.


9. The LFS simulator supports hard links as well. Run the following to study how they work: ./lfs.py -L c,/foo:l,/foo,/bar:l,/foo,/goo -o -i. What blocks are written out when a hard link is created? How is this similar to just creating a new file, and how is it different? How does the reference count field change as links are created?
Answer: The written out blocks are the ones that contains the links. The similarity is that we change the data block that has the link to add the link name. The difference is that we don't need to write a block with new information, type, size, etc. Increases.


10. LFS makes many different policy decisions. We do not explore many of them here – perhaps something left for the future – but here is a simple one we do explore: the choice of inode number. First, run ./lfs.py -p c100 -n 10 -o -a s to show the usual behavior with the ”sequential” allocation policy, which tries to use free inode numbers nearest to zero. Then, change to a ”random” policy by running ./lfs.py -p c100 -n 10 -o -a r (the -p c100 flag ensures 100 percent of the random operations are file creations). What on-disk differences does a random policy versus a sequential policy result in? What does this say about the importance of choosing inode numbers in a real LFS?
Answer: With the random policy the checkpoint holds more pointers to imaps, consequently there is more blocks allocated.


11. One last thing we’ve been assuming is that the LFS simulator always updates the checkpoint region after each update. In the real LFS, that isn’t the case: it is updated periodically to avoid long seeks. Run ./lfs.py -N -i -o -s 1000 to see some operations and the intermediate and final states of the file system when the checkpoint region isn’t forced to disk. What would happen if the checkpoint region is never updated? What if it is updated periodically? Could you figure out how to recover the file system to the latest state by rolling forward in the log? 
Answer: If the checkpoint region is never updated, if some write operation is interrupted, we can't redo it.
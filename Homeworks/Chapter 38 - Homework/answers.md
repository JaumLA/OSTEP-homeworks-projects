1. Use the simulator to perform some basic RAID mapping tests. Run with different levels (0, 1, 4, 5) and see if you can figure out the mappings of a set of requests. For RAID-5, see if you can figure out the difference between left-symmetric and left-asymmetric layouts. Use some different random seeds to generate different problems than above.
Answer: 
Level 0:
	LOGICAL READ from addr:1343 size:4096
		read disk 3, offset 335	
	LOGICAL WRITE to  addr:7637 size:4096
		write disk 1, offset 1909
Level 1:
	LOGICAL READ from addr:1343 size:4096
		read disk 3, offset 671	
	LOGICAL WRITE to  addr:7637 size:4096
		write disk: 2, offset 3818, write disk 3, offset 3818
Level 4:
	LOGICAL READ from addr:1343 size:4096
		read disk 2, offset 447
	LOGICAL WRITE to  addr:7637 size:4096
		read disk 2, offset 2545, read disk 3, offset 2545
		write disk 2, offset 2545, write disk 3, offset 2545
Level 5:
	LOGICAL READ from addr:1343 size:4096
		read disk 3, offset 447
	LOGICAL WRITE to  addr:7637 size:4096
		read disk 1, offset 2545, read disk 2, offset 2545
		write disk 1, offset 2545, write disk 2, offset 2545

2. Do the same as the first problem, but this time vary the chunk size with -C. How does chunk size change the mappings?
Answer: Using 8k of chunk
Level 0:
	LOGICAL READ from addr:1343 size:4096
		read disk 3, offset 335	
	LOGICAL WRITE to  addr:7637 size:4096
		write disk 2, offset 1909
Level 1:
	LOGICAL READ from addr:1343 size:4096
		read disk 1, offset 671	
	LOGICAL WRITE to  addr:7637 size:4096
		write disk: 0, offset 3819, write disk 1, offset 3819
Level 4:
	LOGICAL READ from addr:1343 size:4096
		read disk 2, offset 447
	LOGICAL WRITE to  addr:7637 size:4096
		read disk 2, offset 2545, read disk 3, offset 2545
		write disk 2, offset 2545, write disk 3, offset 2545
Level 5:
	LOGICAL READ from addr:1343 size:4096
		read disk 3, offset 447
	LOGICAL WRITE to  addr:7637 size:4096
		read disk 2, offset 2545, read disk 3, offset 2545
		write disk 2, offset 2545, write disk 3, offset 2545

Change the rows we are accessing. Every row has (chunksize)/(blocksize)*#disks blocks


3. Do the same as above, but use the -r flag to reverse the nature of each problem.
Answer:
Level 0:
	LOGICAL OPERATION is read addr: 7933 size: 4096
		read disk 1, offset 1983
	LOGICAL OPERATION is write addr: 4850 size: 4096
		write disk 2, offset 1212
Level 1:
	LOGICAL OPERATION is read addr: 7934 size: 4096
		read disk 2, offset 3966
	LOGICAL OPERATION is write addr: 4850 size: 4096
		write disk 0, offset 2425, write disk 1, offset 2425
Level 4:
	LOGICAL OPERATION is addr:7933 size: 4096
		read disk 1, offset 2644
	LOGICAL OPERATION is addr: 4850 size: 4096
		read disk 2, offset 1616, read disk 3, offet 1616
		write disk 2, offset 1616, write disk 3, offet 1616
Level 5:
	LOGICAL OPERATION is addr: 9560 size: 4096
		read disk 0, offset 3186
	LOGICAL OPERATION is addr: 565 size: 4096
		read disk 1, offset 188, read disk 3, offet 188
		write disk 1, offset 188, write disk 3, offet 188


4. Now use the reverse flag but increase the size of each request with the -S flag. Try specifying sizes of 8k, 12k, and 16k, while varying the RAID level. What happens to the underlying I/O pattern when the size of the request increases? Make sure to try this with the sequential workload too (-W sequential); for what request sizes are RAID-4 and RAID-5 much more I/O efficient?
Answer: More disks are accessed on each operation.


5. Use the timing mode of the simulator (-t) to estimate the performance of 100 random reads to the RAID, while varying the RAID levels, using 4 disks.
Answer:


6. Do the same as above, but increase the number of disks. How does the performance of each RAID level scale as the number of disks increases?
Answer:

7. Do the same as above, but use all writes (-w 100) instead of reads. How does the performance of each RAID level scale now? Can you do a rough estimate of the time it will take to complete the workload of 100 random writes?
Answer:

8. Run the timing mode one last time, but this time with a sequential workload (-W sequential). How does the performance vary with RAID level, and when doing reads versus writes? How about when varying the size of each request? What size should you write to a RAID when using RAID-4 or RAID-5?
Answer:

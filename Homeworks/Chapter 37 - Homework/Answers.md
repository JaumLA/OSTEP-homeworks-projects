1. Compute the seek, rotation, and transfer times for the following sets of requests: -a 0, -a 6, -a 30, -a 7,30,8, and finally -a 10,11,12,13.
Answer: 
-a 0
    Seek: 0, Rotate: 165, Transfer:30 , Total: 195;

-a 6
    Seek: 0, Rotate: 345, Transfer: 30, Total: 375;

-a 30
    Seek: 80, Rotate: 265, Transfer: 30, Total: 375;

-a 7,30,8
    Seek: 160, Rotate: 545, Transfer: 90, Total: 795;

-a 10,11,12,13
    Seek: 40, Rotate: 425, Transfer: 120, Total: 585;

2. Do the same requests above, but change the seek rate to different values: -S 2, -S 4, -S 8, -S 10, -S 40, -S 0.1. How do the times change?
Answer: 
-a 0
    Seek: 0, Rotate: 165, Transfer:30 , Total: 195, for -S 2, 4, 8, 10, 40, 0.1;

-a 6
    Seek: 0, Rotate: 345, Transfer: 30, Total: 375, for -S 2, 4, 8, 10, 40 and 0.1;

-a 30
    Seek: 40, Rotate: 305, Transfer: 30, Total: 375, for -S 2;
    Seek: 20, Rotate: 315, Transfer: 30, Total: 375, for -S 4;
    Seek: 10, Rotate: 335, Transfer: 30, Total: 375, for -S 8;
    Seek: 8,  Rotate: 337, Transfer: 30, Total: 375, for -S 10;
    Seek: 2,  Rotate: 343, Transfer: 30, Total: 375, for -S 40;
    Seek: 800, Rotate: 265 , Transfer: 30, Total: 1095, for -S 0.1;

-a 7,30,8
    Seek: 80, Rotate: 625, Transfer: 90, Total: 795, for -S 2;
    Seek: 40, Rotate: 305, Transfer: 90, Total: 435, for -S 4;
    Seek: 20, Rotate: 325, Transfer: 90, Total: 435, for -S 8;
    Seek: 16, Rotate: 329, Transfer: 90, Total: 435, for -S 10;
    Seek: 4,  Rotate: 341, Transfer: 90, Total: 435, for -S 40;
    Seek: 1600, Rotate: 545, Transfer: 90, Total: 2235, for -S 0.1;


-a 10,11,12,13
    Seek: 20, Rotate: 445, Transfer: 120, Total: 585, for -S 2;
    Seek: 10, Rotate: 455, Transfer: 120, Total: 585, for -S 4;
    Seek: 5, Rotate: 460, Transfer: 120, Total: 585, for -S 8;
    Seek: 4, Rotate: 461, Transfer: 120, Total: 585, for -S 10;
    Seek: 1, Rotate: 464, Transfer: 120, Total: 585, for -S 40;
    Seek: 400, Rotate: 425, Transfer: 120, Total: 945, for -S 0.1;


3. Do the same requests above, but change the rotation rate: -R 0.1, -R 0.5, -R 0.01. How do the times change?
Answer:
-a 0
    Seek: 0, Rotate: 1650, Transfer: 300, Total: 1950, for -R 0.1;
    Seek: 0, Rotate: 330, Transfer: 60, Total: 390, for -R 0.5;
    Seek: 0, Rotate: 16500, Transfer: 3000, Total: 19500, for -R 0.01;

-a 6
    Seek: 0, Rotate:  330*10 + 15*10, Transfer: 300, Total: , for -R ;
    Seek: 0, Rotate: , Transfer: 300, Total: , for -R ;
    Seek: 0, Rotate: , Transfer: 300, Total: , for -R ;

-a 30
    Seek: , Rotate: , Transfer: , Total: , for -R ;
    Seek: , Rotate: , Transfer: , Total: , for -R ;
    Seek: , Rotate: , Transfer: , Total: , for -R ;

-a 7,30,8
    Seek: , Rotate: , Transfer: , Total: , for -R ;
    Seek: , Rotate: , Transfer: , Total: , for -R ;
    Seek: , Rotate: , Transfer: , Total: , for -R ;

-a 10,11,12,13
    Seek: , Rotate: , Transfer: , Total: , for -R ;
    Seek: , Rotate: , Transfer: , Total: , for -R ;
    Seek: , Rotate: , Transfer: , Total: , for -R ;

4. FIFO is not always best, e.g., with the request stream -a 7,30,8, what order should the requests be processed in? Run the shortest seek-time first (SSTF) scheduler (-p SSTF) on this workload; how long should it take (seek, rotation, transfer) for each request to be served?
Answer: Should be run 7,8,30. 
for 7: Seek: 0, Rotate: 15, Transfer: 30, Total: 45;
for 8: Seek: 0, Rotate: 0, Transfer: 30, Total: 30;
for 30: Seek: 80, Rotate: 230, Transfer: 30, Total: 300;
Total: 375.


5. Now use the shortest access-time first (SATF) scheduler (-p SATF). Does it make any difference for -a 7,30,8 workload? Find a set of requests where SATF outperforms SSTF; more generally, when is SATF better than SSTF?
Answer: For -a 7,30,8 makes no difference. When the longest track counting the rotation has a lower time than the closest track, like -a 8,19,26.


6. Here is a request stream to try: -a 10,11,12,13. What goes poorly when it runs? Try adding track skew to address this problem (-o skew). Given the default seek rate, what should the skew be to maximize performance? What about for different seek rates (e.g., -S 2, -S 4)? In general, could you write a formula to figure out the skew?
Answer: The sectors are sequential but separated on two tracks, so it will need at least a 360 units time because the seek will never get it on time. The formula is the time to get to the sequential sector on a other track, so every 30 unit times needed for seek, we need to increase skew by 1. S_k(S) = floor(S/3)

7. Specify a disk with different density per zone, e.g., -z 10,20,30, which specifies the angular difference between blocks on the outer, middle, and inner tracks. Run some random requests (e.g., -a -1 -A 5,-1,0, which specifies that random requests should be used via the -a -1 flag and that five requests ranging from 0 to the max be generated), and compute the seek, rotation, and transfer times. Use different random seeds. What is the bandwidth (in sectors per unit time) on the outer, middle, and inner tracks?
Answer: 


8. A scheduling window determines how many requests the disk can examine at once. Generate random workloads (e.g., -A 1000,-1,0, with different seeds) and see how long the SATF scheduler takes when the scheduling window is changed from 1 up to the number of requests. How big of a window is needed to maximize performance? Hint: use the -c flag and don’t turn on graphics (-G) to run these quickly. When the scheduling window is set to 1, does it matter which policy you are using?
Answer: The window need to be at least 1/5 of the total requests. No doesn't matter.

9. Create a series of requests to starve a particular request, assuming an SATF policy. Given that sequence, how does it perform if you use a bounded SATF (BSATF) scheduling approach? In this approach, you specify the scheduling window (e.g., -w 4); the scheduler only moves onto the next window of requests when all requests in the current window have been serviced. Does this solve starvation? How does it perform, as compared to SATF? In general, how should a disk make this trade-off between performance and starvation avoidance?
Answer: -a 1,2,3,4,5,6,7,8,9,10,1,2,3...,35. 35 is the process starved. The starvation is solved because the process to be starved will be put together within a window. SATF outperforms the BSTF. Choosing a window size to satisfy it's needs.


10. All the scheduling policies we have looked at thus far are greedy; they pick the next best option instead of looking for an optimal schedule. Can you find a set of requests in which greedy is not optimal?
Answer: 


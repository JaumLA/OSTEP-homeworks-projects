1. Compute the response time and turnaround time when running three jobs of length 200 with the SJF and FIFO schedulers.
Answer: SJF turnaround time is (200 + 400 + 600)/3 = 400 and response time is (0 + 200 + 400)/3 = 200.
FIFO turnaround time is (200 + 400 + 600)/3 = 400 and response time is (0 + 200 + 400)/3 = 200


2. Now do the same but with jobs of different lengths: 100, 200, and 300.
Answer: SJF turnaround time is (100 + 300 + 600)/3 = 333.333 and response time is (0 + 100 + 300)/3 = 133.33.
FIFO turnaround time is (100 + 300 + 600)/3 = 333.333 and response time is (0 + 100 + 300)/3 = 133.33


3. Now do the same, but also with the RR scheduler and a time-slice
of 1.
Answer: RR turnaround time is (298 + 499 + 600)/3 = 465.67 and response time is (0 + 1 + 2)/3 = 1


4. For what types of workloads does SJF deliver the same turnaround
times as FIFO?
Answer: A schedule with processes of same length or a schedule of processes in increasing order of length.


5. For what types of workloads and quantum lengths does SJF deliver the same response times as RR?
Answer: RR has the same response time if the quantum lengths is equal to the response time of the SJF.


6. What happens to response time with SJF as job lengths increase? Can you use the simulator to demonstrate the trend?
Answer: It increases linearly to the job lengths.


7. What happens to response time with RR as quantum lengths increase? Can you write an equation that gives the worst-case response time, given N jobs?
Answer: O(N)

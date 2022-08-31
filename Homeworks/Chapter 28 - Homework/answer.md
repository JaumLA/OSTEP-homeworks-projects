1. Examine flag.s. This code “implements” locking with a single memory flag. Can you understand the assembly?
 Answer: Yes.


2. When you run with the defaults, does flag.s work? Use the -M and -R flags to trace variables and registers (and turn on -c to see their values). Can you predict what value will end up in flag?
Answer: Zero.


3. Change the value of the register %bx with the -a flag (e.g., -a bx=2,bx=2 if you are running just two threads). What does the code do? How does it change your answer for the question above? 
Answer: The same thing. No.


4. Set bx to a high value for each thread, and then use the -i flag to generate different interrupt frequencies; what values lead to a bad outcomes? Which lead to good outcomes?
Answer:For bx=10, 11, 15, 16, 21 works, everything else with -i <= 20 doesn't work.


5. Now let’s look at the program test-and-set.s. First, try to understand the code, which uses the xchg instruction to build a simple locking primitive. How is the lock acquire written? How about lock release?
Answer: The lock aquire is just one instruction that set mutex to one and get the old value of it. Lock release just set mutex to zero.


6. Now run the code, changing the value of the interrupt interval (-i) again, and making sure to loop for a number of times. Does the code always work as expected? Does it sometimes lead to an inefficient use of the CPU? How could you quantify that?
Answer:The code is always correct, but when the lock is free and xchg is called and there is an interrupt, the thread that wants the lock will just loop waiting, wasting time. Take the number of interrupts after xchg and multiply by the interrupt value.



7. Use the -P flag to generate specific tests of the locking code. For example, run a schedule that grabs the lock in the first thread, but then tries to acquire it in the second. Does the right thing happen? What else should you test?
Answer: Yes. Should test the tests after the lock is aquired.



8. Now let’s look at the code in peterson.s, which implements Peterson’s algorithm (mentioned in a sidebar in the text). Study the code and see if you can make sense of it.
Answer: Ok



9. Now run the code with different values of -i. What kinds of different behavior do you see? Make sure to set the thread IDs appropriately (using -a bx=0,bx=1 for example) as the code assumes it.
Answer:



10. Can you control the scheduling (with the -P flag) to “prove” that the code works? What are the different cases you should show hold? Think about mutual exclusion and deadlock avoidance.
Answer:



11. Now study the code for the ticket lock in ticket.s. Does it match the code in the chapter? Then run with the following flags: -a bx=1000,bx=1000 (causing each thread to loop through the critical section 1000 times). Watch what happens; do the threads spend much time spin-waiting for the lock?
Answer:Yes. Yes, they do the critical section one time, and then spin.


12. How does the code behave as you add more threads?
Answer: More threads will spin before the thread whose turn is will run.


13. Now examine yield.s, in which a yield instruction enables one thread
to yield control of the CPU (realistically, this would be an OS primitive, but for the simplicity, we assume an instruction does the task). Find a scenario where test-and-set.s wastes cycles spinning, but yield.s does not. How many instructions are saved? In what scenarios do these savings arise?
Answer: -P 00011111111111111100000



14. Finally, examine test-and-test-and-set.s. What does this lock do? What kind of savings does it introduce as compared to test-and-set.s?
Answer:
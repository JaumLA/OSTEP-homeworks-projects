1. First let’s use a tiny address space to translate some addresses. Here’s a simple set of parameters with a few different random seeds; can you translate the addresses?
    segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 0
    segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 1
    segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 2

Anwser:
Translation for: segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 0
    ARG seed 0
    ARG address space size 128
    ARG phys mem size 512
    
    Segment register information:
        Segment 0 base  (grows positive) : 0x00000000 (decimal 0)
        Segment 0 limit                  : 20

        Segment 1 base  (grows negative) : 0x00000200 (decimal 512)
        Segment 1 limit                  : 20

    Virtual Address Trace
        VA  0: 0x0000006c (decimal:  108) --> Valid in SEG1: 0x000001ec (decimal:  492)
        VA  1: 0x00000061 (decimal:   97) --> SEGMENTATION VIOLATION(SEG1)
        VA  2: 0x00000035 (decimal:   53) --> SEGMENTATION VIOLATION(SEG0)
        VA  3: 0x00000021 (decimal:   33) --> SEGMENTATION VIOLATION(SEG0)
        VA  4: 0x00000041 (decimal:   65) --> SEGMENTATION VIOLATION(SEG1)

Translations for: segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 1
    ARG seed 1
    ARG address space size 128
    ARG phys mem size 512

    Segment register information:
        Segment 0 base  (grows positive) : 0x00000000 (decimal 0)
        Segment 0 limit                  : 20

        Segment 1 base  (grows negative) : 0x00000200 (decimal 512)
        Segment 1 limit                  : 20

    Virtual Address Trace
        VA  0: 0x00000011 (decimal:   17) --> Valid in SEG0: 0x00000011 (decimal:   17)?
        VA  1: 0x0000006c (decimal:  108) --> Valid in SEG1: 0x000001EC (decimal:   492)?
        VA  2: 0x00000061 (decimal:   97) --> SEGMENTATION VIOLATION(SEG1)
        VA  3: 0x00000020 (decimal:   32) --> SEGMENTATION VIOLATION(SEG0)
        VA  4: 0x0000003f (decimal:   63) --> SEGMENTATION VIOLATION(SEG0)

Translation for: segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 2
    ARG seed 2
    ARG address space size 128
    ARG phys mem size 512

Segment register information:
    Segment 0 base  (grows positive) : 0x00000000 (decimal 0)
    Segment 0 limit                  : 20

    Segment 1 base  (grows negative) : 0x00000200 (decimal 512)
    Segment 1 limit                  : 20

    Virtual Address Trace
        VA  0: 0x0000007a (decimal:  122) --> Valid in SEG0: 0x000001FA (decimal:   506)
        VA  1: 0x00000079 (decimal:  121) --> Valid in SEG0: 0x000001F9 (decimal:   505)
        VA  2: 0x00000007 (decimal:    7) --> Valid in SEG0: 0x00000007 (decimal:   7)
        VA  3: 0x0000000a (decimal:   10) --> Valid in SEG0: 0x0000000A (decimal:   10)
        VA  4: 0x0000006a (decimal:  106) --> SEGMENTATION VIOLATION(SEG1)

2. Now, let’s see if we understand this tiny address space we’ve constructed (using the parameters from the question above). What is the highest legal virtual address in segment 0? What about the lowest legal virtual address in segment 1? What are the lowest and highest illegal addresses in this entire address space? Finally, how would you run segmentation.py with the -A flag to test if you are right?
Anwser: Highest legal virtual address in SEG0 is 19, and the lowest legal virtual address in SEG1 is 108. The lowest illegal is 20 and the highest is 107. putting those addresses.



3. Let’s say we have a tiny 16-byte address space in a 128-byte physical
memory. What base and bounds would you set up so as to get the simulator to generate the following translation results for the specified address stream: valid, valid, violation, ..., violation, valid, valid? Assume the following parameters:
segmentation.py -a 16 -p 128 -A 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
--b0 ? --l0 ? --b1 ? --l1 ?
Anwser: --b0 0 --l0 2 --b1 16 --l1 2


4. Assume we want to generate a problem where roughly 90% of the randomly-generated virtual addresses are valid (not segmentation violations). How should you configure the simulator to do so? Which parameters are important to getting this outcome?
Anwser: l0 = addresssize.*2/5, b1 = addresssize + b0, l1 == addressize*2/5


5. Can you run the simulator such that no virtual addresses are valid?
How?
Anwser: Just using the register limits with 0.

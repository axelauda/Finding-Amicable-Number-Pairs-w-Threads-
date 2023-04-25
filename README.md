# Finding-Amicable-Number-Pairs-w-Threads-

<h2>Description</h2>
<p>
  In this C project, I developed a program using multi-threading that would go through a range of numbers and find an amicable pair within that range. Amciable numbers are a pair of numbers where the sum of the proper divisors of each is equal to the other number. For example, the proper divisors of 220 are 1, 2, 4, 5, 10, 11, 20, 22, 44, 55, 110 which sum to 284. The proper divisors of 284 are 1, 2, 4, 71, 142 which sum to 220. Thus, 220 and 284 are said to be
“amicable”.
</p>
<b>
  In the program, I would get user input, where an upper limit was chosen to decide the range in which the program checks numbers for amicability. A thread count was also chosen to show the time difference for if one thread, two threads, and more would decrease the time of the program. These inputs were error checked so the limits and thread counts would be reasonable.
</b>
   I also created an algorithm that would find the divisors using a for loop that would sum them as they are found. However, I optimized it so that instead of checking (using a for loop) from one to the number being checked to figure out if it is divisor or not, my algorithm would divide out the divisor that is found, and continuously decrease the range of the for loop to effectively save time.
   In terms of multi-threading, I provided mutual exclusion for all parts of my code that were considered critical sections. For example, when a thread grabs a number to check for amicability, it checks a global variable, and then increments it for the next thread to check the following numbers. To avoid a race condition where multiple threads could grab the same number simultaneously and waste time, I put a mutex lock on that section of code to avoid that problem. There was also another mutex lock I added when a thread would place an amicable pair into an array that will be outputted at the end of the program. This would ensure that there was no possiblity for a thread to overwrite an element of the array if multiple threads were writing at the same time.
</b>


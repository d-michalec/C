Creates 10,000 threads that write to a fixed size chunk of a file
using pwrite.  Of those concurrent threads, a semaphore will allow 
five concurrent threads to contend for a single mutex lock.  Once 
acquiring the lock, a thread can write its chunk of data to the 
file. Similarly, 10,000 threads are created to read a fixed size 
chunk of data from the same file.  The reader threads similarly
 wait on a semaphore and contend for a mutex lock before reading.

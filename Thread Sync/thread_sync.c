/*****************************************************************************
/ PROGRAM:  thread_sync.c
/ AUTHOR:      ****  Damian Michalec ****
/ COURSE:   CS431 - Operating Systems
/ SYNOPSIS: Creates 10,000 threads that write to a fixed size chunk of a file
/           using pwrite.  Of those concurrent threads, a semaphore will allow 
/           five concurrent threads to contend for a single mutex lock.  Once 
/           acquiring the lock, a thread can write its chunk of data to the 
/           file. Similarly, 10,000 threads are created to read a fixed size 
/           chunk of data from the same file.  The reader threads similarly
/           wait on a semaphore and contend for a mutex lock before reading.
/
/           To build, you must manually link the pthread library:
/
/           gcc -o thread_sync thread_sync.c -lpthread -lrt
/
/           (c) Regis University
/*****************************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>

#define CONCURRENT_THREADS 1000
#define DATA_SIZE          5

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static sem_t           semaphore;
static int             fd;
static int             bytes_produced_consumed = 0;

// This is the function called by each writer thread.  Each thread will take
// the following steps:
//
// 1. Construct a char array containing the thread's ID number. The content must
// be justified to exactly five characters regardless of the number of digits.
// Hint: consider using sprintf.
// 2. Call wait on the semaphore
// 3. Acquire the mutex lock
// 4. Do a pwrite of the char array on the file at the the thread's designated offset
// 5. Increment the value of bytes_produced_consumed by DATA_SIZE number of bytes
// 6. Release the mutex lock
// 7. Call signal on the semaphore
void* writer_thread(void* arg) {
  int  thread_id = *(int*)arg;
  char buffer[DATA_SIZE+1];
  int value;

  // Step 1. Create a char array containing only the last four digits of the thread's ID
  sprintf(buffer, "%04d", thread_id); 
  
  // Step 2. Call wait on the semaphore
  sem_wait(semaphore);
  printf("Writer thread %d called semaphore wait\n", thread_id);

  // Step 3. Acquire mutex lock
  pthread_mutex_lock(&mutex);
  printf("Writer thread %d acquired mutex lock\n", thread_id);
  
  // Steps 4 & 5. Execute critical section
  pwrite(fd ,buffer, DATA_SIZE, DATA_SIZE * thread_id);
  bytes_produced_consumed += DATA_SIZE;

  // Step 6. Release mutex lock
  pthread_mutex_unlock(&mutex);
  printf("Writer thread %d called semaphore signal\n", thread_id);

  // Step 7. Call signal on the semaphore
  sem_post(semaphore);
  
  printf("Writer thread %d released mutex lock\n", thread_id);
  
}

// This is the function called by each reader thread.  Each thread will take
// the following steps:
//
// 1. Call wait on the semaphore
// 2. Acquire the mutex lock
// 4. Do a pread of the char array on the file at the the thread's designated offset
// 5. Decrement the value of bytes_produced_consumed by DATA_SIZE number of bytes
// 5. Release the mutex lock
// 6. Call signal on the semaphore
// 7. Output the result of the pread
void* reader_thread(void* arg) {
  int  thread_id = *(int*)arg;
  char buffer[DATA_SIZE];
  int result;

  // Step 1. Call wait on the semaphore
  sem_wait(semaphore);
  printf("Reader thread %d called semaphore wait\n", thread_id);

  // Step 2. Acquire mutex lock
  pthread_mutex_lock(&mutex);
  printf("Reader thread %d acquired mutex lock\n", thread_id);
  
  // Steps 3 & 4. Critical section code 
  result = pread(fd ,buffer, DATA_SIZE, DATA_SIZE * thread_id);
  bytes_produced_consumed =- DATA_SIZE;
	 
  // Step 5. Release mutex lock
  pthread_mutex_unlock(&mutex);
  printf("Reader thread %d called semaphore signal\n", thread_id);

  // Step 6. Call signal on the semaphore
  sem_post(semaphore);
  printf("Reader thread %d released mutex lock\n", thread_id);

  // Step 7. Output the results of the read
  printf("Result: %d\n", result);
  
}

int main() {
  // Initialize semaphore with a value of 5
  sem_init(&semaphore, 0, 5);

  // Create an array large enough to contain 10,000 threads
  pthread_t rthreads[CONCURRENT_THREADS];
  pthread_t wthreads[CONCURRENT_THREADS];

  // Open the file that constitutes the shared buffer for reading and writing
  fd = open("sharedfile.txt", O_RDWR | O_CREAT, (mode_t)0644);
  
  if (fd < 0) {
    fprintf(stderr, "Could not open file sharedfile.txt: %s\n", strerror(errno));
    return EXIT_FAILURE;
  }

  // Create 10,000 writer threads
  for (int i = 0; i < CONCURRENT_THREADS; i++) {
    pthread_create(&wthreads[i], NULL, writer_thread, (void*)&i);
    usleep(10); // Ugly hack to deal with a clock issue on some platforms
  }
  
  // Wait on all writer threads
  for (int i = 0; i < CONCURRENT_THREADS; i++) {
    pthread_join(wthreads[i], NULL);
  }

  // Create 10,000 reader threads
  for (int i = 0; i < CONCURRENT_THREADS; i++) {
    pthread_create(&rthreads[i], NULL, reader_thread, (void*)&i);
    usleep(10);
  }
  
  // Wait on all reader threads
  for (int i = 0; i < CONCURRENT_THREADS; i++) {
    pthread_join(rthreads[i], NULL);
  }

  // Tear down the semaphore and close the file
  sem_destroy(&semaphore);
  close(fd);
  
  return EXIT_SUCCESS;
}

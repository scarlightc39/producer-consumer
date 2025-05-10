#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE 2		// # of items in table, 2 by default
#define SHM_KEY 0x1234	// Shared memory key, 0x1234 by default
#define ITERATIONS 10	// # of times processes will attempt critical section

struct shmbuf {
	sem_t mutex;	// Semaphore for mutual exclusion
	sem_t empty;	// Semaphore to check if table is empty
	int table[SIZE];
};

void* consumer_thread(void* arg) {
	
	printf("Consumer thread created.\n");
	
	time_t t;
	srand((unsigned) time(&t));		// used to create random integer (producer)
	int shm;
	
	struct shmbuf *shmptr;
	
	shm = shmget(SHM_KEY, sizeof(struct shmbuf), 0644|IPC_CREAT);	// create shared memory buffer
	if (shm == -1) printf("SHM key error\n");
	
	shmptr = shmat(shm, NULL, 0);					// attach to shared memory buffer
	if (shm == -1) printf("SHM attach error\n");
	
	sleep(1);
	
	for (int it = 0; it < ITERATIONS; it++) {
	
		sleep(1);
		sem_wait(&shmptr->mutex);			// Calling wait on "mutex" to ensure mutual exclusion
		printf("Consumer entered...\n");		// Entered critical section
		
		int i, j = 1;					// i = empty's value, j = table index
		sem_getvalue(&shmptr->empty, &i);
		while (i < SIZE) {			// while i < size of table...
			
			int x;
			x = shmptr->table[j];		// x = integer from table[j]
			sem_post(&shmptr->empty);	// declare that an item has been taken from the table
			printf("Consumed item %d\n", j);	
			printf("Item %d consumed = ", j);	// output what it got to check with producer output
			printf("%d\n", x);
			++j;
			sem_getvalue(&shmptr->empty, &i);	// check i again to re-evaluate while loop
			
		}

		printf("Consumer done consuming items since the table is empty.\n");

		sem_post(&shmptr->mutex);	// no longer in critical section, so post "mutex"
	}
	
	printf("Consumer thread has finished working.\n");
	
	shmdt(shmptr);			// detach shared memory buffer
	shmctl(shm, IPC_RMID, NULL);	// remove shared memory buffer
}

int main() {


	pthread_t consumer;
	pthread_create (&consumer, NULL, consumer_thread, NULL);	// create consumer thread defined above
	
	pthread_join(consumer, NULL);					// start thread

	return 0;
}

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

void* producer_thread(void* arg) {

	printf("Producer thread created.\n");
	
	time_t t;
	srand((unsigned) time(&t));		// used to create random integer (producer)
	int shm;
	
	struct shmbuf *shmptr;
	
	shm = shmget(SHM_KEY, sizeof(struct shmbuf), 0644|IPC_CREAT);	// create shared memory buffer
	if (shm == -1) printf("SHM key error\n");
	
	shmptr = shmat(shm, NULL, 0);					// attach to shared memory buffer
	if (shm == -1) printf("SHM attach error\n");
	
	sem_init(&shmptr->mutex, 1, 1);					// initialize mutex semaphore to 1
	sem_init(&shmptr->empty, 1, SIZE);				// initialize empty semaphore to <SIZE>
	
	for (int it = 0; it < ITERATIONS; it++) {
	
		sleep(1);
		sem_wait(&shmptr->mutex);			// Calling wait on "mutex" to ensure mutual exclusion
		printf("Producer entered...\n");		// Entered critical section
		
		int i, j = 1;					// i = empty's value, j = table index
		sem_getvalue(&shmptr->empty, &i);
		while (i > 0) {				// while i > 0 (while table is not empty)...
		
			int x = rand();			// make x a random integer
			shmptr->table[j] = x;		// table[j] = random integer(x)
			sem_wait(&shmptr->empty);	// declare that an item has been put into the table
			printf("Produced item %d\n", j);
			printf("Item %d produced = ", j);	// output what it put in to check with consumer output
			printf("%d\n", x);
			++j;
			sem_getvalue(&shmptr->empty, &i);	// check i again to re-evaluate while loop

		}
		
		printf("Producer done producing items since the table is full.\n");
		
		sem_post(&shmptr->mutex);	// no longer in critical section, so post "mutex"
	
	}
	
	printf("Producer thread has finished working.\n");
	
	sem_destroy(&shmptr->mutex);	// destroy semaphores
	sem_destroy(&shmptr->empty);
	shmdt(shmptr);			// detach shared memory buffer
}

int main() {
	
	pthread_t producer;
	pthread_create (&producer, NULL, producer_thread, NULL);	// create producer thread defined above
	
	pthread_join(producer, NULL);					// start thread
	
	return 0;
}

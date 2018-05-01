/*
Code is written by team 48: B05902101 & B05902102
It's about creating 2 threads. Each thread will print who is running & busy for 0.5 seconds.

We will work on two types of scheduling methods:
1. Default (non-real time) 		: sudo ./sched_test
2. Real-time scheduling policy	: sudo ./sched_test SCHED_FIFO
*/
#define _GNU_SOURCE
#include<linux/sched.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h> // buat sleep (busy here)
#include<pthread.h>
#include<time.h>
#include<sched.h>

cpu_set_t set_CPU;	/* set 1 or more bits for CPU & each number 1 on the bit represents # of CPU */

void *thread_func(void *id_passed) {
	int h;
	int number_of_thread = *((int*)id_passed);
	for(h = 0; h < 3; h++) {
		printf("Thread %d is running\n",number_of_thread+1);

		/* BUSY 0.5 SECONDS ??? */
		clock_t start, end;
		start = clock();
		end = clock();
		while((double)(end-start)/CLOCKS_PER_SEC <= 0.5) { /* CLOCKS_PER_SEC is used to convert clock_t type to sec & CLOCKS_PER_SEC  = 1e6 */
			end = clock();
		}
		//printf("Increasing time = %f\n",(double)(end-start)/CLOCKS_PER_SEC);
	}
}

int main(int argc, char const *argv[]) {
	if((argc < 1 && argc > 2) || (argc == 2 && strcmp(argv[1], "SCHED_FIFO") != 0)) {
		fprintf(stderr, "Usage: ./sched_test or ./sched_test SCHED_FIFO\n");
		return -1;
	} 

	CPU_ZERO(&set_CPU);	/* clear the set_CPU / empty it */
	CPU_SET(0, &set_CPU); /* set CPU 0 to work */
	sched_setaffinity(0, sizeof(set_CPU), &set_CPU); 	/* set CPU Affinity, which is all threads run on the same core */

	if(argc == 2 && strcmp(argv[1], "SCHED_FIFO") == 0) { /* the permission to run real-time process */
		struct sched_param param;
		param.sched_priority = sched_get_priority_max(SCHED_FIFO);	/* invoke FIFO_SCHED whose priority value at 1 to 99 & set the priority of real-time process */
		int s = sched_setscheduler(0, SCHED_FIFO, &param);	/* if pid=0,then caller's thread will be set to the corresponding policy and sched_param */
		if(s != 0) {
			fprintf(stderr, "Function sched_setscheduler error\n");
		}
	}
	
	int i, j;
	pthread_t tid[2];

	for(i = 0; i < 2; i++) {
		int *thread_number = (int *)malloc(sizeof(int));
		*thread_number = i;
		/* pthread_create(pthread_t type address, attr of thread, start func, param of start func) */
		if(pthread_create(&tid[i],NULL,(void *)thread_number) != 0) {	/* create thread */
			fprintf(stderr, "Error in creating a thread\n");
			return -1;
		} else printf("Thread %d was created\n",i+1);
	}

	for(j = 0; j < 2; j++) {
		pthread_join(tid[j], NULL);	/* wait for thread to exit */
	}

	return 0;
}

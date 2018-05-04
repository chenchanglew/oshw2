#define _GNU_SOURCE


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <time.h>
#include <memory.h>
#include <pthread.h>

void taskSet(int coreid)
{
	int ret;
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(coreid, &set);
	ret = sched_setaffinity(0, sizeof(set), &set);

	if (ret < 0) {
		perror("sched_setaffinity");
		exit(-1);
	}
}

void busy(const double wait)
{
	clock_t start, end;
	start = clock();
	end = clock();

	while ( (  ( (double)(end - start) ) / CLOCKS_PER_SEC  ) < wait )
		end = clock();
}

void *thread_func(void *arg)
{
	int i;
	long t_num = (long) arg;
	for (i = 0; i < 3; i++) {
		printf("Thread %ld is running\n", t_num);
		busy(0.5);
	}
	pthread_exit(NULL);
}

void invoke_fifo(void)
{
	int ret;
	struct sched_param sp;
	memset(&sp, 0, sizeof(sp));
	sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
	ret = sched_setscheduler(0, SCHED_FIFO, &sp);
	if (ret < 0) {
		perror("sched_setscheduler");
		exit(-1);
	}
}

int main(int argc, char *argv[])
{
	int i;
	pthread_t tid1, tid2;
	taskSet(0);
	
	if (argc == 2 && strcmp(argv[1], "SCHED_FIFO") == 0)	invoke_fifo();

	pthread_create(&tid1, NULL, thread_func, (void *)1);
	printf("Thread %d was created\n", 1);

	pthread_create(&tid2, NULL, thread_func, (void *)2);
	printf("Thread %d was created\n", 2);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	return 0;
}
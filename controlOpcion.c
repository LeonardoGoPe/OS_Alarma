// C program sensor simulation
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

#define SHMSZ     4  

void main(){
	int shmidA,*shmA;
	if ((shmidA = shmget(7788, SHMSZ,  0666)) < 0) {
		perror("shmget");
		return(1);
	}
	if ((shmA = shmat(shmidA, NULL, 0)) == (int *) -1) {
		perror("shmat");
		return(1);
	}
	printf("%d",*shmA);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "user.h"
#include <semaphore.h>
sem_t *semaphore;
UserList* sharedData;

int main() {
	semaphore=malloc(sizeof(sem_t));
	sem_init(semaphore, 0, 1);
	int shmid = createSharedMemory(sharedData);
	sharedData =(UserList*) attachSharedMemory(shmid);
	sharedData = initUserList();
	char commande[100];
	mkfifo("pipe1",0666);
	mkfifo("pipe2",0666);
	while (1) {
		printf("Commande : ");
		fgets(commande, sizeof(commande), stdin);
		commande[strcspn(commande, "\n")] = '\0';
		interprete_commande(commande,sharedData,semaphore);
	}
	detachSharedMemory(sharedData);
	destroySharedMemory(shmid);
	free(semaphore);
	return 0;
}


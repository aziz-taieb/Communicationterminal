#include <stdio.h>
#define MAX_USERS 100
#define MAX_NAME_LENGTH 50
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "user.h"
#include <semaphore.h>
int createSharedMemory(UserList * sharedData) {
    int shmid = shmget(SHM_W, sizeof(sharedData), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Erreur lors de la création de la mémoire partagée");
        exit(1);
    }
    return shmid;
}
UserList* attachSharedMemory(int shmid) {
    UserList* data = (UserList*)shmat(shmid, NULL, 0);
    if (data == (void*)-1) {
        perror("Erreur lors de l'attachement de la mémoire partagée");
        exit(1);
    }
    return data;
}
UserList* initUserList() {
    UserList* userList =malloc(sizeof(UserList));
    userList->numUsers = 0;
    userList->numConnectedUsers=0;
    return userList;
}
void detachSharedMemory(UserList* data) {
    if (shmdt(data) == -1) {
        perror("Erreur lors du détachement de la mémoire partagée");
        exit(1);
    }
}

void destroySharedMemory(int shmid) {
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("Erreur lors de la suppression de la mémoire partagée");
        exit(1);
    }
}
void listConnectedUsers(UserList * sharedData) {
     printf("Utilisateurs connectés :\n");
     for (int i = 0; i < sharedData->numUsers; i++) {
        printf("%d. %s\n", i + 1, sharedData->users[i].name);
    }
}

void disconnectUser(const char* name,UserList *sharedData) {
    for (int i = 0; i < sharedData->numUsers; i++) {
        if (strcmp(name, sharedData->users[i].name) == 0) {
            for (int j = i; j < sharedData->numUsers - 1; j++) {
                sharedData->users[j] = sharedData->users[j + 1];
            }
            sharedData->numUsers--;
            sharedData->numConnectedUsers--;
            printf("L'utilisateur '%s' s'est déconnecté.\n", name);
            return;
        }
    }
    printf("Utilisateur non trouvé : '%s'.\n", name);
}

void detruire(UserList*u){
    free(u);
}
int  isUserInList(const char * name, UserList userList) {
    for (int i = 0; i < userList.numUsers; i++) {
        if (strcmp(name, userList.users[i].name) == 0) {
            return 1 ;
        }
    }
    return 0;
}

int  addUser(UserList* userList, const char* name) {
    if (userList->numUsers >= MAX_USERS) {
        printf("Le nombre maximum d'utilisateurs est atteint.\n");
        return -1;
    }
    if(isUserInList(name, *(userList))){
    	printf("L'utilisateur est déja connecté ! \n");
	return -1;
    }
    User newUser;
    snprintf(newUser.name, MAX_NAME_LENGTH, "%s", name);
    userList->users[userList->numUsers] = newUser;
    userList->numUsers++;
    return 1;
}

void printUsers(const UserList* userList) {
    printf("Liste des utilisateurs :\n");
    for (int i = 0; i < userList->numUsers; i++) {
  	  printf("%d. %s\n", i + 1, userList->users[i].name);
    }
}
void enregistrement(const char* nom_utilisateur,UserList * p) {
    if(addUser(p,nom_utilisateur)!=-1){
	    printf("L'utilisateur '%s' est enregistré.\n", nom_utilisateur);
    }
}
void ouvrir_dialogue(const char* n1,const char* n2) {

    pid_t pid1, pid2;
   
    pid1 = fork();
    if (pid1 == 0) {
        execl("/usr/bin/xterm", "xterm", "-e", "./dialogue",n1,"0", NULL);
        exit(0);
    }
   
    pid2 = fork();
    if (pid2 == 0) {
        execl("/usr/bin/xterm", "xterm", "-e", "./dialogue",n2,"1", NULL);
        exit(0);
    }
   
    printf("Dialogue ouvert avec  <'%s'> et <'%s'>.\n", n1,n2);
}

void quitter() {
    printf("Le PMI a été quitté.\n");
    exit(0);
}
void decomposerMots(const char* chaine, char** mot1, char** mot2) {
    *mot1 = (char*)malloc(strlen(chaine) + 1); 
    *mot2 = (char*)malloc(strlen(chaine) + 1);

    int index = 0;
    while (chaine[index] != ' ' && chaine[index] != '\0') {
        (*mot1)[index] = chaine[index];
        index++;
    }
    (*mot1)[index] = '\0'; 

    if (chaine[index] == ' ') {
        index++; 
    }

    strcpy(*mot2, chaine + index); 
}
int containsSpace(const char* str) {
    while (*str != '\0') {
        if (*str == ' ') {
            return 1;
        }
        str++;
    }
    return 0;
}
void interprete_commande(const char* commande, UserList * p , sem_t* semaphore) {
	char **n1=malloc(sizeof(char*));
	char **n2= malloc(sizeof(char*));
	if (commande[0]=='e') {
		sem_wait(semaphore);
		enregistrement(commande+2,p);
		sem_post(semaphore);
	}else if (commande[0]== 'p') {
		
		if(!containsSpace(commande+2)){
			printf("Commande Invalide : <Usage> p : User1 user2 <usage> \n");
			return;
		}
		decomposerMots(commande + 2,n1,n2);
		if(isUserInList(*n1,*p) && isUserInList(*n2,*p)){	
			if(strcmp(*n1,*n2)==0){
				printf("Pas de dialogue avec la même personne\n");
				return;
			}
			ouvrir_dialogue(*n1,*n2);
		}else{
			printf("L'un des deux utilisateur n'est pas enregistré ! \n");
			return;
		}
	}else if(commande[0] == 'l'){ 
		sem_wait(semaphore);
		listConnectedUsers(p);
		sem_post(semaphore);
	}else if (commande[0] == 'd') {
			if (strlen(commande) <= 2) {
				printf("Commande invalide : d <nom_utilisateur>\n");
				return;
			}
			sem_wait(semaphore);
			disconnectUser(commande + 2,p);
			sem_post(semaphore);
	} else if (strcmp(commande, "q") == 0) {
                                 quitter();
				 return;
        }
}

	

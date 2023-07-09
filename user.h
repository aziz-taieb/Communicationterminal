#ifndef USER_H
#define USER_H

#define MAX_USERS 100
#define MAX_NAME_LENGTH 50
#include <semaphore.h>
typedef struct {
    char name[MAX_NAME_LENGTH];
} User;

typedef struct {
    User users[MAX_USERS];
    int numUsers;
    int numConnectedUsers;
} UserList;



UserList*initUserList() ;
int  isUserInList(const char * name, UserList userList) ;
void detruire(UserList*u);
int addUser(UserList* userList, const char* name);
void printUsers(const UserList* userList);
void detachSharedMemory(UserList* data) ;
void destroySharedMemory(int shmid); 
void enregistrement(const char* nom_utilisateur,UserList * p);
void ouvrir_dialogue(const char* n1,const char* n2) ;
void quitter();
void interprete_commande(const char* commande, UserList * p, sem_t * semaphore);
UserList* attachSharedMemory(int shmid);
int createSharedMemory();
#endif /* USER_H */


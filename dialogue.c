#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 100

void* readThread(void* arg) {
   int test=*(int*)arg;
   int piperead;
   if(test==0){
      piperead=open("pipe1",O_RDONLY);
   }else{
      piperead=open("pipe2",O_RDONLY);
   }
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    while ((bytesRead = read(piperead, buffer, sizeof(buffer) - 1)) > 0) {
        if (strcmp(buffer, "/quitter") == 0) {
            printf("Le destinataire a checkout \n");
            break;
        }
        printf("Destinataire : %s\n",buffer);
        buffer[bytesRead] = '\0';
    }


    return NULL;
}

void* writeThread(void* arg) {
   int test=*(int*)arg;
   int pipewrite;
    if(test==0){
      pipewrite=open("pipe2",O_WRONLY);
   }else{
      pipewrite=open("pipe1",O_WRONLY);
   }

    char message[BUFFER_SIZE];

    while (strcmp(message, "/quitter") != 0) {
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0'; // Supprimer le saut de ligne

        // Écrire le message dans le premier tube
        write(pipewrite, message, strlen(message) + 1);
    }


    return NULL;
}

int main(int argc ,char*argv[]) {
    int k=atoi(argv[2]);
    pthread_t tid1, tid2;
    printf("Votre destinataire est %s\n",argv[1]);
    
    if (pthread_create(&tid1, NULL, readThread,&k)!=0) {
        perror("pthread_create");
        exit(1);
    }

    if (pthread_create(&tid2, NULL, writeThread, &k) != 0) {
        perror("pthread_create");
        exit(1);
    }

    if (pthread_join(tid2, NULL) != 0) {
        perror("pthread_join");
        exit(1);
    }

    pthread_cancel(tid1);

    if (pthread_join(tid1, NULL) != 0) {
        perror("pthread_join");
        exit(1);
    }

    return 0;
}

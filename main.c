#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include "list.h"

#define MSG_MAX_LEN 4
unsigned short PORT1;
unsigned short PORT2;
char* hostName;


List* send_list;
List* display_list;

//Receive Condition Variables
static pthread_cond_t receiveCondVar = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t receiveMutex = PTHREAD_MUTEX_INITIALIZER;

//Send Condition Variables
static pthread_cond_t sendCondVar = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t sendMutex = PTHREAD_MUTEX_INITIALIZER;


int socketDescriptor1, socketDescriptor2;
void* keyboardInputThread(void* args) { 
    char* message = (char*)malloc(MSG_MAX_LEN);
    while (1) {

        
        fgets(message, MSG_MAX_LEN, stdin);
        char* messageToSave = strdup(message); 
        fputs("\n", stdout);
        if(strncmp(messageToSave, "!", 1)== 0 && strlen(messageToSave) == 2) {
            fputs("Terminating the dialog...\n", stdout);
            exit(0);
        }
        List_append(send_list, messageToSave);

        pthread_mutex_lock(&sendMutex);
        {
            pthread_cond_signal(&sendCondVar);
        }
        pthread_mutex_unlock(&sendMutex);

    }

    return NULL;
}

void* receiveThread(void* args) {   
    int socketDescriptor = *((int*)args);
    // char messageRx[MSG_MAX_LEN];


    while (1) {
        char* messageRx = (char*)malloc(MSG_MAX_LEN);
        struct sockaddr_in sinRemote;
        unsigned int sin_len = sizeof(sinRemote);
        int byteReceived = recvfrom(socketDescriptor, messageRx, MSG_MAX_LEN, 0, (struct sockaddr*) &sinRemote, &sin_len);
        //char* messageToSave = strdup(messageRx); 
        //printf("Message received: %s\n", messageRx);
        messageRx[byteReceived] = '\0';
        List_append(display_list, messageRx);

        pthread_mutex_lock(&receiveMutex);
        {
            pthread_cond_signal(&receiveCondVar);
        }
        pthread_mutex_unlock(&receiveMutex);
    }

    return NULL;
}

void* sendThread(void* args) {
   
    int socketDescriptor = *((int*)args);

    while (1) {

        pthread_mutex_lock(&sendMutex);
        {
                pthread_cond_wait(&sendCondVar, &sendMutex);
                
        }
        pthread_mutex_unlock(&sendMutex);

        // char messageTx[MSG_MAX_LEN];
        char* messageTx = (char*)List_trim(send_list);

            struct addrinfo hints, *res;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_DGRAM;
            

            struct sockaddr_in sinRemote; //cs-srye4013ue02
            getaddrinfo(hostName,NULL, &hints, &res);
            memcpy(&sinRemote, res->ai_addr, res->ai_addrlen);
            // sinRemote.sin_family = AF_INET;
            // inet_pton(AF_INET, , &sinRemote.sin_addr);
            sinRemote.sin_port = htons(PORT2); //socketDescriptor == socketDescriptor1 ? PORT2 : PORT1);

            sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr*) &sinRemote, sizeof(sinRemote));
            free(messageTx); // Free the memory after sending the message


    }

    return NULL;
}

void* screenOutputThread(void* args) {
   
    while (1) {

        pthread_mutex_lock(&receiveMutex);
        {
            pthread_cond_wait(&receiveCondVar, &receiveMutex);
        }
        pthread_mutex_unlock(&receiveMutex);

        fputs("Received: ", stdout);
        char* message = (char*)List_trim(display_list);
        if (message) {
            fputs(message, stdout);
            fputs("\n", stdout);
            if(strncmp(message, "!", 1)== 0 && strlen(message) == 2) {
                fputs("Terminating the dialog...", stdout);
                exit(0);
            }
            free(message); // Free the memory after displaying the message
        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4)
    {
        fputs("Wrong input. Usage: %s <PORT1 Remote_Machine_Name PORT2>\n", stdout);
        exit(0);
    }
    PORT1 = (unsigned short) atoi(argv[1]);
    hostName = argv[2];
    PORT2 = (unsigned short) atoi(argv[3]);
    fputs("Starting the dialog...\n", stdout);

    send_list = List_create();
    display_list = List_create();

    // Create sockets for communication
    socketDescriptor1 = socket(PF_INET, SOCK_DGRAM, 0);

    // Bind sockets to the respective ports
    struct sockaddr_in sin1;
    memset(&sin1, 0, sizeof(sin1));
    sin1.sin_family = AF_INET;
    sin1.sin_addr.s_addr = htonl(INADDR_ANY);
    sin1.sin_port = htons(PORT1);
    bind(socketDescriptor1, (struct sockaddr*)&sin1, sizeof(sin1));


    pthread_t receiveThreadPID1, sendThreadPID1, keyboard_thread, screen_output;

    
    pthread_create(&sendThreadPID1, NULL, sendThread, &socketDescriptor1); 
    pthread_create(&receiveThreadPID1, NULL, receiveThread, &socketDescriptor1);
    pthread_create(&screen_output, NULL, screenOutputThread, NULL);
    pthread_create(&keyboard_thread, NULL, keyboardInputThread, NULL);
    
    


    pthread_join(keyboard_thread, NULL);
    pthread_join(sendThreadPID1, NULL);
    pthread_join(receiveThreadPID1, NULL);
    pthread_join(screen_output, NULL);


    close(socketDescriptor1);

    List_free(send_list, free);
    List_free(display_list, free);

    fputs("Done\n", stdout);
    return 0;
}

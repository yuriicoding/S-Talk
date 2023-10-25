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

#define MSG_MAX_LEN 1024
#define PORT1 6060
#define PORT2 6001

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
        List_append(send_list, messageToSave);

        //Node* iter = send_list->head;
        // while (iter != NULL){
        //     //printf("Something");
        //     char* item_val = (char*)iter->value;
        //     printf("%s\n", item_val);
        //     iter = iter->pointerNext;
            
        // }


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
    char* messageRx = (char*)malloc(MSG_MAX_LEN);

    while (1) {
        char* messageToSave = strdup(messageRx); 
        //char* message = (char*)List_trim(send_list);
        struct sockaddr_in sinRemote;
        unsigned int sin_len = sizeof(sinRemote);
        recvfrom(socketDescriptor, messageRx, MSG_MAX_LEN, 0, (struct sockaddr*) &sinRemote, &sin_len);
        //printf("Message received: %s\n", messageRx);
        List_append(display_list, messageToSave);

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
        if (messageTx) {
            printf("Enter a message to send (or type 'q' to quit): ");
            // fgets(messageTx, MSG_MAX_LEN, stdin);

            if (strcmp(messageTx, "q\n") == 0) {
                break;
            }

            struct addrinfo hints, *res;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_DGRAM;
            
            

        
            struct sockaddr_in sinRemote;
            getaddrinfo("asb9838nu-e07",NULL, &hints, &res);
            memcpy(&sinRemote, res->ai_addr, res->ai_addrlen);
            // sinRemote.sin_family = AF_INET;
            // inet_pton(AF_INET, , &sinRemote.sin_addr);
            sinRemote.sin_port = htons(socketDescriptor == socketDescriptor1 ? PORT2 : PORT1);

            sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr*) &sinRemote, sizeof(sinRemote));
            free(messageTx); // Free the memory after sending the message
        }

    }

    close(socketDescriptor);
    pthread_cancel(pthread_self());

    return NULL;
}

void* screenOutputThread(void* args) {
    while (1) {

        pthread_mutex_lock(&receiveMutex);
        {
            pthread_cond_wait(&receiveCondVar, &receiveMutex);
        }
        pthread_mutex_unlock(&receiveMutex);

        char* message = (char*)List_trim(display_list);
        if (message) {
            printf("%s", message);
            free(message); // Free the memory after displaying the message
        }
    }

    return NULL;
}

int main() {
    printf("Starting...\n");
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
    pthread_create(&keyboard_thread, NULL, keyboardInputThread, NULL);
    pthread_create(&sendThreadPID1, NULL, sendThread, &socketDescriptor1);

    
    pthread_create(&receiveThreadPID1, NULL, receiveThread, &socketDescriptor1);
    pthread_create(&screen_output, NULL, screenOutputThread, NULL);


    pthread_join(sendThreadPID1, NULL);
    pthread_cancel(receiveThreadPID1);
    pthread_join(receiveThreadPID1, NULL);
    pthread_join(keyboard_thread, NULL);
    pthread_join(screen_output, NULL);

    
    List_free(send_list, free);
    List_free(display_list, free);

    printf("Done\n");
    return 0;
}

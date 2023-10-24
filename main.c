#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define MSG_MAX_LEN 1024
#define PORT1 6060
#define PORT2 6001

int socketDescriptor1, socketDescriptor2;

void* receiveThread(void* args) {
    int socketDescriptor = *((int*)args);
    char messageRx[MSG_MAX_LEN];

    while (1) {
        struct sockaddr_in sinRemote;
        unsigned int sin_len = sizeof(sinRemote);
        recvfrom(socketDescriptor, messageRx, MSG_MAX_LEN, 0, (struct sockaddr*) &sinRemote, &sin_len);
        printf("Message received: %s\n", messageRx);
    }

    return NULL;
}

void* sendThread(void* args) {
    int socketDescriptor = *((int*)args);

    while (1) {
        char messageTx[MSG_MAX_LEN];
        printf("Enter a message to send (or type 'q' to quit): ");
        fgets(messageTx, MSG_MAX_LEN, stdin);

        if (strcmp(messageTx, "q\n") == 0) {
            break;
        }

        struct sockaddr_in sinRemote;
        sinRemote.sin_family = AF_INET;
        inet_pton(AF_INET, "127.0.0.1", &sinRemote.sin_addr);
        sinRemote.sin_port = htons(socketDescriptor == socketDescriptor1 ? PORT2 : PORT1);

        sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr*) &sinRemote, sizeof(sinRemote));
    }

    close(socketDescriptor);
    pthread_cancel(pthread_self());

    return NULL;
}

int main() {
    printf("Starting...\n");

    // Create sockets for communication
    socketDescriptor1 = socket(PF_INET, SOCK_DGRAM, 0);
    socketDescriptor2 = socket(PF_INET, SOCK_DGRAM, 0);

    // Bind sockets to the respective ports
    struct sockaddr_in sin1, sin2;
    memset(&sin1, 0, sizeof(sin1));
    sin1.sin_family = AF_INET;
    sin1.sin_addr.s_addr = htonl(INADDR_ANY);
    sin1.sin_port = htons(PORT1);
    bind(socketDescriptor1, (struct sockaddr*)&sin1, sizeof(sin1));

    memset(&sin2, 0, sizeof(sin2));
    sin2.sin_family = AF_INET;
    sin2.sin_addr.s_addr = htonl(INADDR_ANY);
    sin2.sin_port = htons(PORT2);
    bind(socketDescriptor2, (struct sockaddr*)&sin2, sizeof(sin2));

    pthread_t receiveThreadPID1, sendThreadPID1, receiveThreadPID2, sendThreadPID2;
    pthread_create(&receiveThreadPID1, NULL, receiveThread, &socketDescriptor1);
    pthread_create(&sendThreadPID1, NULL, sendThread, &socketDescriptor1);
    pthread_create(&receiveThreadPID2, NULL, receiveThread, &socketDescriptor2);
    pthread_create(&sendThreadPID2, NULL, sendThread, &socketDescriptor2);

    pthread_join(sendThreadPID1, NULL);
    pthread_cancel(receiveThreadPID1);
    pthread_join(receiveThreadPID1, NULL);
    pthread_join(sendThreadPID2, NULL);
    pthread_cancel(receiveThreadPID2);
    pthread_join(receiveThreadPID2, NULL);

    printf("Done\n");

    return 0;
}

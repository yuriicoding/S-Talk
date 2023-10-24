#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define MSG_MAX_LEN 1024
#define PORT 22110

int main(){
    printf("Yurii's Net Listen Test on UDP port %d:\n", PORT);
    printf("Connect using: \n");
    printf("    netcat -u 127.0.0.1 %d\n", PORT);

    //Address
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(PORT);

    //Create socket for UDP
    int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

    //Bind socket to the port
    bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

    while(1) {

        struct sockaddr_in sinRemote;
        unsigned int sin_len = sizeof(sinRemote);
        char messageRx[MSG_MAX_LEN];
        int byteRx = recvfrom(socketDescriptor,
            messageRx, MSG_MAX_LEN, 0, 
            (struct sockaddr *) &sinRemote, &sin_len);

        int terminateIdx = (byteRx < MSG_MAX_LEN) ? byteRx : MSG_MAX_LEN - 1;
        messageRx[terminateIdx] = 0;
        printf("Message received (%d bytes): \n\n'%s'\n", byteRx, messageRx);

        int incMe = atoi(messageRx);
        char messageTx[MSG_MAX_LEN];
        sprintf(messageTx, "Math: %d + 1 = %d\n", incMe, incMe + 1);

        sin_len = sizeof(sinRemote);
        sendto( socketDescriptor, 
            messageTx, strlen(messageTx), 0,
            (struct sockaddr *) &sinRemote, sin_len);
            


    }

    close(socketDescriptor);
    return 0;
}


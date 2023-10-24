#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <arpa/inet.h>
#define MSG_MAX_LEN 1024
#define PORT 22110

// Define a node for the message list
typedef struct Node {
    char message[MSG_MAX_LEN];
    struct Node *next;
} Node;

// Define the message list
typedef struct {
    Node *head;
    Node *tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond_var;
} MessageList;

MessageList sendList, receiveList;

// Function to initialize a message list
void init_message_list(MessageList *list) {
    list->head = NULL;
    list->tail = NULL;
    pthread_mutex_init(&list->mutex, NULL);
    pthread_cond_init(&list->cond_var, NULL);
}

// Function to add a message to a list
void add_message(MessageList *list, const char *message) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    strcpy(newNode->message, message);
    newNode->next = NULL;

    pthread_mutex_lock(&list->mutex);

    if (list->tail == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }

    pthread_cond_signal(&list->cond_var);
    pthread_mutex_unlock(&list->mutex);
}

// Function to get and remove a message from a list
char *get_message(MessageList *list) {
    pthread_mutex_lock(&list->mutex);

    while (list->head == NULL) {
        pthread_cond_wait(&list->cond_var, &list->mutex);
    }

    Node *tempNode = list->head;
    char *message = strdup(tempNode->message);

    list->head = list->head->next;
    if (list->head == NULL) {
        list->tail = NULL;
    }

    free(tempNode);

    pthread_mutex_unlock(&list->mutex);

    return message;
}

// Thread function to get input from the keyboard
void *keyboard_input_thread(void *arg) {
    while (1) {
        char message[MSG_MAX_LEN];
        fgets(message, MSG_MAX_LEN, stdin);
        add_message(&sendList, message);
    }
    return NULL;
}

// Thread function to send UDP messages
// TODO: Complete this function to send messages over UDP
void *udp_send_thread(void *arg) {
    // Assuming arg contains the necessary information (IP and port)
    struct sockaddr_in sinRemote;
    // ... (Initialize sinRemote using data from arg)
    memset(&sinRemote, 0, sizeof(sinRemote));
    sinRemote.sin_family = AF_INET;
    sinRemote.sin_addr.s_addr = htonl(INADDR_ANY);
    sinRemote.sin_port = htons(PORT);
    int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

    while (1) {
        char *message = get_message(&sendList);

        sendto(socketDescriptor, message, strlen(message), 0,
               (struct sockaddr *)&sinRemote, sizeof(sinRemote));

        free(message);
    }

    close(socketDescriptor);
    return NULL;
}
// TODO: Implement the UDP receive and screen output threads

int main(int argc, char *argv[]) {
    pthread_t tid_keyboard, tid_sender;

    // Initialize the message lists
    init_message_list(&sendList);
    init_message_list(&receiveList);

    // Create the threads
    pthread_create(&tid_keyboard, NULL, keyboard_input_thread, NULL);
    pthread_create(&tid_sender, NULL, udp_send_thread, NULL);

    // Wait for the threads to finish
    pthread_join(tid_keyboard, NULL);
    pthread_join(tid_sender, NULL);

    return 0;
}

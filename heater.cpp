#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

struct msgBuffer {
    long msgType;
    char msgContent[100];
} message;


int main() {
    int washingTemp[] = {40, 35, 45, 30, 50};
    int mode = 0;
    string operation;
    key_t myKey;
    int msgID;
    msgID = msgget(myKey, 0666 | IPC_CREAT); //create message queue and return id
    
    /* *** RECEIVING *** */
    printf("Waiting to close the door\n");
    msgrcv(msgID, &message, sizeof(message), 1, 0);
    printf("Received mode: %s \n", message.msgContent);
    //usleep(500000);

    do {
        printf("Waiting to close the door\n");
        msgrcv(msgID, &message, sizeof(message), 1, 0);
        usleep(500000);
    } while (strcmp(message.msgContent, "Door closed"));
    printf("Door closed!\n");

    do {
        printf("Waiting to get mode\n");
        msgrcv(msgID, &message, sizeof(message), 1, 0);
        usleep(500000);
    } while (!strcmp(message.msgContent, "Door closed"));
    printf("Received mode: %s \n", message.msgContent); 
    mode = stoi(message.msgContent);
    
    
    /* *** PREPARING *** */
        
    int waterHeated = washingTemp[mode-1];
    int i = 0;
    printf("Heating water.");
    while (i < waterHeated) {
        printf(".");
        usleep(1000000); 
        i++;
    };
        
    printf("\n");
    printf("Water heated successfully!\n");
    
    /* *** SENDING *** */
    
    message.msgType = 2;
    myKey = ftok("progfile", 66); //create unique key
    msgID = msgget(myKey, 0666 | IPC_CREAT); //create message queue and return id
    strncpy(message.msgContent, "Water heated", sizeof(message.msgContent));
    
    usleep(5000000); 
    msgsnd(msgID, &message, sizeof(message), 0); //send message
    printf("Sent message is : %s \n", message.msgContent);
    msgctl(msgID, IPC_RMID, NULL); //destroy the message queue
}
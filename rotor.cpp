#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <string>
#include <unistd.h>

using namespace std;

struct msgBuffer {
    long msgType;
    char msgContent[100];
} message;


int main() {
    int washingTime[] = {90, 115, 30, 65, 70};
    string operation;
    key_t myKey;
    int msgID;
    msgID = msgget(myKey, 0666 | IPC_CREAT); //create message queue and return id

    printf("Waiting for pump to pump water.\n\n");
    
    /* *** RECEIVING *** */
    
    msgrcv(msgID, &message, sizeof(message), 3, 0);
    printf("Received Message is : %s \n", message.msgContent);
    
    if (strcmp(message.msgContent, "") == 0) {
}
    usleep(washingTime[0]*100000);   

    /* *** PREPARING *** */

    int waterHeated = 4;
    printf("Washing dirty clothes.");
    for (int i = 1; i <= waterHeated; i++) {
        printf(".");

    }
    printf("\n");
    printf("Washing completed!\n");
    
    /* *** SENDING *** */
    
    message.msgType = 4;
    myKey = ftok("progfile", 67); //create unique key
    msgID = msgget(myKey, 0666 | IPC_CREAT); //create message queue and return id
    strncpy(message.msgContent, "Washing completed", sizeof(message.msgContent));
    
    usleep(5000000);
    msgsnd(msgID, &message, sizeof(message), 0); //send message
    printf("Sent message is : %s \n", message.msgContent);
    msgctl(msgID, IPC_RMID, NULL); //destroy the message queue
}
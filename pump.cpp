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
    string operation;
    key_t myKey;
    int msgID;

    printf("Waiting for heater to heat water.\n");
    
    /* *** RECEIVING *** */
    
    msgrcv(msgID, &message, sizeof(message), 2, 0);
    //msgctl(msgID, IPC_RMID, NULL); //destroy the message queue
    printf("Received Message is : %s \n", message.msgContent);
    usleep(5000000);   

    /* *** PREPARING *** */

    int waterPumped = 4;
    printf("Pumping water.");
    for (int i = 1; i <= waterPumped; i++) {
        printf(".");
        
    }
    printf("\n");
    printf("Water pumped successfully!\n");
    
    /* *** SENDING *** */
    
    message.msgType = 3;
    myKey = ftok("progfile", 67); //create unique key
    msgID = msgget(myKey, 0666 | IPC_CREAT); //create message queue and return id
    strncpy(message.msgContent, "Water pumped", sizeof(message.msgContent));
    
    usleep(5000000); 
    msgsnd(msgID, &message, sizeof(message), 0); //send message
    printf("Sent message is : %s \n", message.msgContent);
    msgctl(msgID, IPC_RMID, NULL); //destroy the message queue
}
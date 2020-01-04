#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

struct msgBuffer {
    long msgType;
    char msgContent[100];
} message;


int main() {
    int choose;
    string operation = "";
    key_t myKey;
    int msgID;
    
    /* *** SENDING *** */ 
    
    myKey = ftok("/usr", 65); //create unique key
    msgID = msgget(myKey, 0666 | IPC_CREAT); //create message queue and return id
    
    message.msgType = 1;
    
    do {
        printf("Close the door [closed]: ");
        getline(cin, operation);
    } while (operation.compare("closed"));
    operation = "Door closed";

    
    strcpy(message.msgContent, operation.c_str());   
    msgsnd(msgID, &message, sizeof(message), 0); //send message
    printf("\nSent message: %s \n", message.msgContent); 


    /* *** PREPARING *** */ 

    usleep(2000000); 
    printf("Choose washing mode [0-4] \n");
    printf("    [0] Ordinary wash:   40'C and 90min\n");
    printf("    [1] Sport's wash:    35'C and 80min\n");
    printf("    [2] Shirt wash:      45'C and 115min\n");
    printf("    [3] Hand wash:       30'C and 30min\n");
    printf("    [4] Synthetics wash: 50'C and 70min \n\n");
    cout << "Type here: "; cin >> choose;

    printf("\nYou have choosen ");
    switch(choose) {
        case 0:
            printf("ordinary wash!");
            break;
        case 1:
            printf("sport's wash!");
            break;
        case 2:
            printf("shirt wash!");
            break;
        case 3:
            printf("hand wash mode!");
            break;
        case 4:
            printf("synthetics wash mode!");
            break;
        default:
            printf("ordinary wash!");
            choose = 0;
            break;
    }

    strcpy(message.msgContent, to_string(choose).c_str());   
    msgsnd(msgID, &message, sizeof(message), 0); //send message
    printf("\nSending selected mode to heater: %s \n", message.msgContent);  

    printf("Washing in progress.\n");


    /* *** RECEIVING *** */ 

    myKey = ftok("/boot", 67); //create unique key
    msgID = msgget(myKey, 0666 | IPC_CREAT); //create message queue and return id

    msgrcv(msgID, &message, sizeof(message), 4, 0);
    operation = message.msgContent;
    if(operation.find("fail") != string::npos) {
            printf("Received error message: %s \n", message.msgContent);
            message.msgType = 1;
            myKey = ftok("/usr", 65); //create unique key
            msgID = msgget(myKey, 0666 | IPC_CREAT); //create message queue and return id
            msgsnd(msgID, &message, sizeof(message), 0); //send message
            
            exit(-1);
    }
    printf("Received message is : %s \n", message.msgContent);
    msgctl(msgID, IPC_RMID, NULL); //destroy the message queue
    usleep(1000000); 
    
    printf("You can open the door now\n");
    printf("Open the door [open]: ");
    
    while (operation.compare("open")) {
        getline(cin, operation);
    } 
    usleep(5000000); 

    printf("Have a nice day with your clean loundry!\n");

}
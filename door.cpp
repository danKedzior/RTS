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
    /*
    Tryby pracy:
        [1] Sport       35' 90min
        [2] Koszule     40' 115min
        [3] Reczne      30' 30min
        [4] Syntetyki   50' 70min
    */
    string operation = "";
    string washingModes[] = {"Sport's wash", "Shirt wash", "Hand wash", "Synthetics wash"};
    key_t myKey;
    int msgID;
    myKey = ftok("progfile", 65); //create unique key
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
    msgrcv(msgID, &message, sizeof(message), 4, 0);
    printf("Received Message is : %s \n", message.msgContent);
    msgctl(msgID, IPC_RMID, NULL); //destroy the message queue
    usleep(5000000); 

    printf("You can open the door now\n");
    do {
        printf("Open the door [open] : ");
        getline(cin, operation);
    } while (operation.compare("open"));
    usleep(5000000); 
    printf("Have a nice day with your clean loundry!\n");
}
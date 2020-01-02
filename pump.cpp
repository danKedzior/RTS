#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <chrono>

using namespace std;

struct msgBuffer {
    long msgType;
    char msgContent[100];
} message;


int main() {
    srand (time(NULL));
    string operation = "";
    key_t myKey;
    int received = 2;
    int msgID;
    int mode = 0;
    int tolerance = (rand() % 3 + 4) * 10000;  //[µs]
    cout << "Tolerance: " << tolerance << endl;

    printf("Waiting for heater to heat water.\n");
    
    /* *** RECEIVING *** */
        
    myKey = ftok("/var", 66); //create unique key
    msgID = msgget(myKey, 0666 | IPC_CREAT); //create message queue and return id
    
    for (int i = 1; i <= received; i++)   {
        msgrcv(msgID, &message, sizeof(message), 2, 0);
        operation = message.msgContent;
        if(operation.find("fail") != string::npos) {
            printf("Received error message: %s \n", message.msgContent);
            message.msgType = 3;
            myKey = ftok("/temp", 67); //create unique key
            msgID = msgget(myKey, 0666 | IPC_CREAT); //create message queue and return id
            msgsnd(msgID, &message, sizeof(message), 0); //send message
            
            exit(-1);
        }
        else if (i==2) {
            printf("Received mode: %s \n", message.msgContent);
            mode = stoi(message.msgContent);//msgctl(msgID, IPC_RMID, NULL); //destroy the message queue
        }
        else {
            printf("Received message: %s \n", message.msgContent);
        }
        usleep(5000000);  
    } 

    
    /* *** PREPARING *** */

    int waterPumped = 3;
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    printf("Pumping water to rotor.");
    for (int i = 0; i <= waterPumped; i++) {
        printf(".");
        usleep(10000); 
    };
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    cout << endl;
    //cout << "\nDifference " << chrono::duration_cast<chrono::microseconds>(end-begin).count() << "[µs]" << endl;
    
    if((chrono::duration_cast<chrono::microseconds>(end-begin).count()) > tolerance) {
        strncpy(message.msgContent, "Pump fail", sizeof(message.msgContent));
        msgsnd(msgID, &message, sizeof(message), 0); //send message

        message.msgType = 3;
        myKey = ftok("/temp", 67); //create unique key
        msgID = msgget(myKey, 0666 | IPC_CREAT); //create message queue and return id
        msgsnd(msgID, &message, sizeof(message), 0); //send message
        printf("Error message: %s \n", message.msgContent);

        exit(-1);
    } 
    else {
        printf("\n");
        printf("Water pumped successfully!\n");
        
        /* *** SENDING *** */
        
        message.msgType = 3;
        myKey = ftok("/temp", 67); //create unique key
        msgID = msgget(myKey, 0666 | IPC_CREAT); //create message queue and return id

        strncpy(message.msgContent, "Water pumped", sizeof(message.msgContent));
        usleep(5000000); 
        msgsnd(msgID, &message, sizeof(message), 0); //send message
        printf("Sent message is: %s \n", message.msgContent);

        strcpy(message.msgContent, to_string(mode).c_str()); 
        usleep(5000000); 
        msgsnd(msgID, &message, sizeof(message), 0); //send message
        printf("Sending selected mode to rotor: %s \n", message.msgContent);

    }
    msgctl(msgID, IPC_RMID, NULL); //destroy the message queue
}
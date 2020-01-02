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
    int washingTemp[] = {40, 35, 45, 30, 50};
    int mode = 0;
    int received = 2;
    string operation;
    key_t myKey;
    int msgID;
    int tolerance = (rand() % 3 + 4) * 10000;  //[µs]
    cout << "Tolerance: " << tolerance << endl;
    /* *** RECEIVING *** */ 

    printf("Waiting to close the door.\n");    
    
    myKey = ftok("/usr", 65); //create unique key
    msgID = msgget(myKey, 0666 | IPC_CREAT); //create message queue and return id
    
    for (int i = 1; i <= received; i++)   {
        msgrcv(msgID, &message, sizeof(message), 1, 0);
        if (i==2) {
            printf("Received mode: %s \n", message.msgContent);
            mode = stoi(message.msgContent);
        }
        else {
            printf("Received message is: %s \n", message.msgContent);
        }
        usleep(5000000);  
    } 

    
    /* *** PREPARING *** */ 

    int waterHeated = 3;
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    printf("Heating water to temperature %d'C", washingTemp[mode]);
    for (int i = 0; i <= waterHeated; i++) {
        printf(".");
        usleep(10000); 
    };
    printf("\n");
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    
    //cout << "\nDifference " << chrono::duration_cast<chrono::microseconds>(end-begin).count() << "[µs]" << endl;
    
    if((chrono::duration_cast<chrono::microseconds>(end-begin).count()) > tolerance) {
        message.msgType = 2;
        myKey = ftok("/var", 66); //create unique key
        msgID = msgget(myKey, 0666 | IPC_CREAT); //create message queue and return id
        strncpy(message.msgContent, "Heater fail", sizeof(message.msgContent));
        msgsnd(msgID, &message, sizeof(message), 0); //send message  
          
        printf("Error message: %s \n", message.msgContent);

        exit(-1);
    } 
    else {
        printf("\n");
        printf("Water heated successfully!\n");
        

        /* *** SENDING *** */
        
        message.msgType = 2;
        myKey = ftok("/var", 66); //create unique key
        msgID = msgget(myKey, 0666 | IPC_CREAT); //create message queue and return id

        strncpy(message.msgContent, "Water heated", sizeof(message.msgContent));
        usleep(5000000); 
        msgsnd(msgID, &message, sizeof(message), 0); //send message
        printf("Sent message is: %s \n", message.msgContent);

        strcpy(message.msgContent, to_string(mode).c_str()); 
        usleep(5000000); 
        msgsnd(msgID, &message, sizeof(message), 0); //send message
        printf("Sending selected mode to pump: %s \n", message.msgContent);

        
        /* *** FAILURE *** */
        myKey = ftok("/usr", 65); //create unique key
        msgID = msgget(myKey, 0666 | IPC_CREAT); //create message queue and return id
        
        msgrcv(msgID, &message, sizeof(message), 1, 0);
        operation = message.msgContent;
        if(operation.find("fail") != string::npos) {
            printf("Received error message: %s \n", message.msgContent);
            message.msgType = 2;
            myKey = ftok("/var", 66); //create unique key
            msgID = msgget(myKey, 0666 | IPC_CREAT); //create message queue and return id
            msgsnd(msgID, &message, sizeof(message), 0); //send message
            
            exit(-1);
    }
        /******************/
    }
    msgctl(msgID, IPC_RMID, NULL); //destroy the message queue
}
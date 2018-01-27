#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <thread>
using namespace std;
//Client side
int clientSd;
void checkerThread()
{
    char recvedMessage[1500];
    while (1)
    {
        memset(recvedMessage, 0, sizeof(recvedMessage));
        recv(clientSd, (char*)&recvedMessage, sizeof(recvedMessage), 0);
        //cout << "Received from server..." << endl;
        if(!strcmp(recvedMessage, "exit"))
        {
            cout << "Server has quit the session" << endl;
            exit(0);
        }
        cout << "Server: " << recvedMessage << endl;
    }
}
int main(int argc, char *argv[])
{
    //we need 2 things: ip address and port number, in that order
    if(argc != 3)
    {
        cerr << "Usage: ip_address port" << endl; exit(0); 
    } //grab the IP address and port number 
    char *serverIp = argv[1]; int port = atoi(argv[2]); 
    //create a message buffer 
    char msg[1500]; 
    //setup a socket and connection tools 
    struct hostent* host = gethostbyname(serverIp); 
    sockaddr_in sendSockAddr;   
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr)); 
    sendSockAddr.sin_family = AF_INET; 
    sendSockAddr.sin_addr.s_addr = 
        inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    clientSd = socket(AF_INET, SOCK_STREAM, 0);
    //try to connect...
    int status = connect(clientSd, (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));

    if(status < 0)
    {
        cout<<"Error connecting to socket!"<<endl; //break;
    }
    cout << "Connected to the server!" << endl;
    thread t1(checkerThread);
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);
    while(1)
    {
        cout << ">";
        string data;
        getline(cin, data);
        memset(&msg, 0, sizeof(msg));//clear the buffer
        strcpy(msg, data.c_str());
        if(data == "exit")
        {
            send(clientSd, (char*)&msg, strlen(msg), 0);
            break;
        }
        send(clientSd, (char*)&msg, strlen(msg), 0);
        //cout << "Awaiting server response..." << endl;
        memset(&msg, 0, sizeof(msg));//clear the buffer
    }
    t1.join();
    gettimeofday(&end1, NULL);
    close(clientSd);
    cout << "Connection closed" << endl;
    return 0;    
}
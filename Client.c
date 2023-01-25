#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>

#define MAXLINE 1024

int main(int argc, char* argv[]){

    int socketFd, uSocketFd, uRecvSocketFd;
    struct sockaddr_in serverAddress, fromAddress, clientAddress;
    char msg[MAXLINE];
    char cpyMsg[MAXLINE];
    char prompt[MAXLINE] = ":~$";
    static char default_prompt[MAXLINE] = ":~$";
    char delim[] = " \n";
    char space[] = " ";
    char* command;
    char* name;
    int rMsgLength;
    socklen_t fromAddrSize = sizeof(serverAddress);
    
    if(argc != 3){
        printf("Usage : %s [IP_address] [Port number]\n", argv[0]);
        exit(1);
    }

    if(atoi(argv[2]) < 0){
        printf("Invalid port number !\n");
        exit(1);
    }
    char *IP = argv[1];
    int port_num = atoi(argv[2]);
    //printf("IP : %s\n", IP);
    //printf("port : %d\n", port_num);

    // Set client address
    bzero(&clientAddress, sizeof(clientAddress));
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_addr.s_addr = inet_addr(IP);
    clientAddress.sin_port = htons(port_num);
    
    // TCP socket creation
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketFd == -1){
        printf("Failed to create TCP socket\n");
        exit(1);
    }else{
        //printf("TCP socket : %d\n", socketFd);
    }

    // UDP socket creation
    uSocketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if(socketFd == -1){
        printf("Failed to create UDP socket\n");
        exit(1);
    }else{
        //printf("UDP socket : %d\n", uSocketFd);
    }


    // TCP connect
    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(IP);
    serverAddress.sin_port = htons(port_num);

    bzero(msg, sizeof(msg));
    if(connect(socketFd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0){
        printf("Failed to connect\n");
        exit(1);
    }else{
        //printf("Client TCP connected");
        read(socketFd, msg, sizeof(msg));
        fputs(msg, stdout);
    }


    while(1){
        // Enter and send user command (TCP or UDP)
        printf("%s", prompt);
        if(fgets(msg, MAXLINE, stdin) == NULL) break;
        strcpy(cpyMsg, msg);
        command = strtok(cpyMsg, delim);

        if(strcmp(msg, "\n")==0)
        {
            continue;
        }

        if(strcmp(command, "exit")==0)
        {
            // Do TCP routine
            write(socketFd, msg, strlen(msg));
            read(socketFd, msg, sizeof(msg));
            fputs(msg, stdout);
            break;
        }
        else if(strcmp(command, "register")==0 || strcmp(command, "game-rule")==0 || strcmp(command, "help")==0)
        {
            
            // Do UDP routine
            //printf("UDP routine entered ...\n");
            sendto(uSocketFd, msg, strlen(msg), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
            rMsgLength = recvfrom(uSocketFd, msg, sizeof(msg), 0, (struct sockaddr*)&serverAddress, &fromAddrSize);
            if(rMsgLength != 0){
                //printf("rMsglength: %d\n", rMsgLength);
                msg[rMsgLength] = 0;
                fputs(msg, stdout);
            }else{
                printf("Nothing received!\n");
            }

        }
        //else if(strcmp(command, "login")==0 || strcmp(command, "logout")==0 || strcmp(command, "start-game")==0)
        else
        {
            write(socketFd, msg, strlen(msg));
            read(socketFd, msg, sizeof(msg));
            fputs(msg, stdout);
            if(strcmp(command, "login")==0 && msg[0] == 'W')
            {
                name = strtok(NULL, space);
                strcat(name, prompt);
                strcpy(prompt, name);
            }
            if(strcmp(command, "logout")==0)
            {
                strcpy(prompt, default_prompt);
            }
        }

    }

    // Close sockets
    close(socketFd);
    //printf("TCP socket closed ...\n");
    close(uSocketFd);
    //printf("UDP socket closed ...\n");
}

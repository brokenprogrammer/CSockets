/**
 * CSockets.
 * C Socket application.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 The CSockets Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "connection_handler.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>

#include "system_control.h"
#include "process.h"

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

char *trimwhitespace(char *str)
{
    char *end;
    
    // Trim leading space
    while(isspace(*str)) str++;
    
    if(*str == 0)  // All spaces?
        return str;
    
    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace(*end)) end--;
    
    // Write new null terminator
    *(end+1) = 0;
    
    return str;
}

/**
 * readCommand
 * Reads string commands and preforms actions depending on what command
 * was specified.
 *
 * @param s - String with command.
 */
void readCommand(char* s, struct processes *processList, int sockfd) {
    char clientMessage[1000];
    printf("Waiting for input %s\n", s);
    
    s = trimwhitespace(s);
    
    if (strcmp(s, "system") == 0) {
        //Make system call
        system_runCommand("ls");
    } else if (strcmp(s, "start app") == 0 || strcmp(s, "start") == 0) {
        //if (send(sockfd, "Enter your target application: ", 31, 0) == -1) {
        //    printf("Error sending message: %s\n", strerror(errno));
        //}
        
        //Recieve reply from client
        //if (recv(sockfd, clientMessage, 1000, 0) < 0) {
        //    printf("Error recieving message: %s\n", strerror(errno));
        //}
        
        //for (int x = 0; x <= strlen(clientMessage); x++) {
        //    printf("%c", clientMessage[x]);
        //}
        
        //Attempt to start VLC with movie in fullscreen.
        system_launchApplication("VLC", processList);
    } else {
        if (send(sockfd, "Unknown command\n", 16, 0) == -1) {
            printf("Error sending message: %s\n", strerror(errno));
        }
    }
}

/**
 * getClientInput
 * Uses recv to recieve messages from the client that has connected and then
 * read the messages.
 *
 * @param sockfd - File descriptor to the connected client socket.
 */
void getClientInput(int sockfd) {
    int readsize;
    char clientMessage[1000];
    
    //Struct to hold application processes used by this connection.
    struct processes *processList = malloc(sizeof(*processList));
    processList->name = NULL;
    processList->pid = -1;
    processList->next = NULL;
    
    //TODO:
    //1. Server - Client Communication. (Type commands from client and server respond)
    //1.5 Rewrite trim string function.
    //2. Start VLC through know commands.
    //3. End connection (Quit through command line).
    //4. Fix warnings & errors.
    
    while (1) {
        while ((readsize = recv(sockfd, clientMessage, 1000, 0)) > 0) {
            for (int x = 0; x < strlen(clientMessage); x++) {
                printf("%c", clientMessage[x]);
            }
            readCommand(clientMessage, processList, sockfd);
            if (send(sockfd, "Welcome", 7, 0) == -1) {
                printf("Error sending welcome message: %s\n", strerror(errno));
            }
        }
    
        if (readsize == 0) {
            printf("Client disconnected\n");
        }
    }
}

/**
 * waitConnection
 * Waits for incoming connections to the server.
 *
 * @param sockfd - Socket file descriptor for the server connection.
 */
void waitConnection (int sockfd) {
    int connectedSock; //Socket connected to this application.
    struct sockaddr_storage their_addr; //Connectors address information.
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN]; //
    
    //Wait for incoming connections.
    while(1) {
        sin_size = sizeof their_addr;
        connectedSock = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        
        if (connectedSock == -1) {
            printf("Error accepting connection: %s\n", strerror(errno));
            continue;
        }
        
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        
        pid_t childProcess = newProcess();
        
        if (childProcess == 0) { //Child process
            close(sockfd);
            
            printf("Connection established with: %s\n", s);
            
            if (send(connectedSock, "Welcome", 7, 0) == -1) {
                printf("Error sending welcome message: %s\n", strerror(errno));
            }
            //readCommand("system");
            getClientInput(connectedSock);
            close(connectedSock);
            
            printf("Success\n");
            destroyProcess();
        } else { // Parrent process
            printf("From Parrent\n");
        }
    }
}
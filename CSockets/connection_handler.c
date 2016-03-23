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

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

/**
 * readCommand
 * Reads string commands and preforms actions depending on what command
 * was specified.
 *
 * @param s - String with command.
 */
void readCommand(char* s) {
    if (strcmp(s, "system") == 0) {
        //Make system call
        system_runCommand("ls");
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
        
        if (!fork()) {
            close(sockfd);
            
            printf("Connection established with: %s\n", s);
            
            if (send(connectedSock, "Welcome", 7, 0) == -1) {
                printf("Error sending welcome message: %s\n", strerror(errno));
            }
            readCommand("system");
            close(connectedSock);
            
            printf("Success\n");
            
            exit(0);
        }
    }
}
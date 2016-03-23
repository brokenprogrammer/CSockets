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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>

#include "process_handler.h"
#include "connection_handler.h"

#define PORT "3490"
#define BACKLOG 10

int main(int argc, const char * argv[]) {
    int sockfd = 0; //Will be used to store file descriptor.
    
    int status;
    struct addrinfo hints; //Criteria used when selecting socket address.
    struct addrinfo *res;  //Linked list that getaddrinfo populates with data.
    struct addrinfo *p;    //Pointer to res used to loop through the recieved data.
    char ipstr[INET6_ADDRSTRLEN]; //String that we will later store an ip address in string form.
    
    int yes = 1;
    struct sigaction sa;
    
    memset(&hints, 0, sizeof hints); //Populates hints with 0 on all the empty positions in the struct.
    hints.ai_family = AF_UNSPEC;     //Doesn't matter if IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; //Use a TCP connection.
    hints.ai_flags = AI_PASSIVE;     //Fill in my address for me.
    
    //Connecting using getaddrinfo and checking if there is an error.
    if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
        //If error exists print it out and exit.
        fprintf(stderr, "Error: %s\n", gai_strerror(status));
        return 1;
    }
    
    printf("IP Adress for: Me\n");
    
    
    for (p = res; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            printf("Error creating socket: %s\n", strerror(errno));
            continue; //This wont be usedso continue to next element in list.
        }
        
        //Attempt to manipulate socket options.
        if ((setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) == -1) {
            printf("Error creating socket options: %s\n", strerror(errno));
            exit(1);
        }
        
        if ((bind(sockfd, p->ai_addr, p->ai_addrlen)) == -1) {
            printf("Error binding: %s\n", strerror(errno));
            continue;
        }
        
        
        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in*)p->ai_addr;
            inet_ntop(p->ai_family, &ipv4->sin_addr, ipstr, sizeof ipstr);
            printf("Server address bound: %s:%s\n", ipstr, PORT);
        }
        
        break;
    }
    
    freeaddrinfo(res); //Free the linked list that we got from getaddrinfo.
    
    if (listen(sockfd, BACKLOG) == -1) {
        printf("Error listening: %s\n", strerror(errno));
    }
    
    //Reep dead proccesses
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        printf("Error running signal action: %s\n", strerror(errno));
        exit(1);
    }
    
    printf("Waiting for connections...\n");
    
    //Accept() loop
    waitConnection(sockfd);
    
    return 0;
}

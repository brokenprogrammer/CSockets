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


#include "process.h"
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>

/**
 * newProcess
 * Creates a new child process and returns the new process id for the child.
 *
 * @returns process id for new child process.
 */
pid_t newProcess() {
    pid_t pid;
    
    pid = fork();
    
    if (pid >= 0) { //Fork successfull
        if (pid == 0) { //If Child
            printf("Connection was successfull. (In Child Process)\n");
        }
    } else {
        //Fork failed
        return -1;
    }
    
    return pid;
}

/**
 * destroyProcess
 * When a process is done we can call this to exit the calling process.
 */
void destroyProcess() {
    exit(0);
}

/**
 *
 */
void pushProcess(struct processes *processList, char* name, pid_t pid) {
    struct processes *newprocess = (struct processes*)malloc(sizeof(*newprocess));
    
    newprocess->name = name;
    newprocess->pid = pid;
}

/**
 *
 */
void popProcess(struct processes *processList) {
    
}

/**
 *
 */
void showActiveProcesses(struct processes *processList) {
    for (; processList; processList = processList->next) {
        printf("%s : %i", processList->name, processList->pid);
    }
}
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

#include "system_control.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

/**
 *
 */
char * build_path() {
    return "";
}

/**
 * system_runCommand
 * Runs target command on the system.
 *
 * @param c - Command to run.
 * @returns -1 on error.
 */
int system_runCommand(char * c) {
    char command[50];
    
    //strcpy(command, "open -a VLC");
    strcpy(command, "echo Hello World");
    
    system(command);
    
    return 0;
}

/**
 * system_launchApplication
 * Launches target application using system commands.
 *
 * @param c - Application to launch.
 * @returns -1 on error.
 */
int system_launchApplication(char* c, struct processes *processList) {
    printf("Launching VLC\n");
    //strcpy(command, "open -a VLC \"/Users/oskarmendel/Music/Red Hot Chilli Peppers - Greatest Hits [Bubanee]\" --args --intf macosx");
    char *const parmList[] = {"/Applications/VLC.app/Contents/MacOS/VLC", "/Users/oskarmendel/Music/Red Hot Chilli Peppers - Greatest Hits [Bubanee]", NULL};
    
    pid_t a = newProcess();
    
    if (a == -1) {
        //Error
        printf("Error occurr\n");
    }
    
    if (a == 0) {
        //Child process
        execv("/Applications/VLC.app/Contents/MacOS/VLC", parmList);
        int errcode = errno;
        exit(errcode); // Should not get here.
    } else {
        // Parent
        
        pushProcess(&processList, "VLC", a);
        showActiveProcesses(processList);
        
        int r;
        waitpid(a, &r, 0);
        
        if (r == 0) {
            printf("Child application terminated successfully\n");
        } else {
            printf("Child application terminated with error\n");
        }
    }

    return 0;
}
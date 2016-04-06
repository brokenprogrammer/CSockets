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

#import "OSXWindowHandler.h"

DL_INTERFACE int setFullscreen(pid_t a) {
    [OSXWindowHandler setFullscreen:a];
    return 1;
}

@implementation OSXWindowHandler

+ (int) setFullscreen: (pid_t) a {
    [NSThread sleepForTimeInterval:2.0f];
    printf("IN OBJC: %i \n \n \n \n \n \n", a);
    
    //SBApplication *vlc = [SBApplication applicationWithProcessIdentifier:a];
    
    //[vlc activate];
    
    NSArray *apps = [[NSWorkspace sharedWorkspace] runningApplications];
    
    for (NSRunningApplication *app in apps) {
        if ([app processIdentifier] == a) {
            printf("Found VLC: %i \n", [app processIdentifier]);
            //[app hide];
            //[app unhide];
            //[NSThread sleepForTimeInterval:2.0f];
            //[app activateWithOptions:0];
            //[app activateWithOptions:1];
            //[app unhide];
        }
    }
    
    NSMutableArray *windows = (NSMutableArray *)CFBridgingRelease(CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements, kCGNullWindowID));
    
    NSMutableDictionary* arr = CFBridgingRelease(CFBridgingRetain(windows));
    for (NSMutableDictionary* entry in arr) {
        pid_t pid = [[entry objectForKey:(id)kCGWindowOwnerPID] intValue];
        
        printf("pid: %i \n", pid);
        
        if (pid == a) {
            printf("SAME PIDS!\n");
            AXUIElementRef elementRef = AXUIElementCreateApplication(pid);
            CFTypeRef position;
            CGPoint point;
            
            AXUIElementCopyAttributeValue(elementRef, kAXPositionAttribute, (CFTypeRef *)&position);
            AXValueGetValue(position, kAXValueCGPointType, &point);
            
            NSLog(@"point=%@", point);
            
            //stackoverflow.com/questions/21069066/move-other-windows-on-mac-os-x-using-accessibility-api
            //stackoverflow.com/questions/614185/window-move-and-rezise-apis-in-os-x
        }
    }

    return 1;
}

@end

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

static bool amIAuthorized ()
{
    if (AXAPIEnabled()) {
        /* Yehaa, all apps are authorized */
        return true;
    }
    /* Bummer, it's not activated, maybe we are trusted */
    if (AXIsProcessTrusted() != 0) {
        /* Good news, we are already trusted */
        return true;
    }
    /* Crap, we are not trusted...
     * correct behavior would now be to become a root process using
     * authorization services and then call AXMakeProcessTrusted() to make
     * ourselves trusted, then restart... I'll skip this here for
     * simplicity.
     */
    return false;
}


static AXUIElementRef getFrontMostApp ()
{
    pid_t pid;
    ProcessSerialNumber psn;
    
    GetFrontProcess(&psn);
    GetProcessPID(&psn, &pid);
    return AXUIElementCreateApplication(pid);
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
    
   /*NSMutableArray *windows = (NSMutableArray *)CFBridgingRelease(CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements, kCGNullWindowID));
    
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
            
            NSLog(@"point.x=%f point.y=%f", point.x, point.y);
            
            CGPoint newPoint;
            newPoint.x = 0;
            newPoint.y = 0;
            
            position = (CFTypeRef)(AXValueCreate(kAXValueCGPointType, (const void *)&newPoint));
            
            AXUIElementSetAttributeValue(elementRef, kAXPositionAttribute, position);

            //stackoverflow.com/questions/21069066/move-other-windows-on-mac-os-x-using-accessibility-api
            //stackoverflow.com/questions/614185/window-move-and-rezise-apis-in-os-x
        }
    }*/
    
    int i;
    AXValueRef temp;
    CGSize windowSize;
    CGPoint windowPosition;
    CFStringRef windowTitle;
    AXUIElementRef frontMostApp;
    AXUIElementRef frontMostWindow;
    
    //if (!amIAuthorized()) {
      //  printf("Can't use accessibility API!\n");
        //return 1;
    //}
    
    NSDictionary *options = @{(id)CFBridgingRelease(kAXTrustedCheckOptionPrompt): @YES};
    BOOL accessibilityEnabled = AXIsProcessTrustedWithOptions((CFDictionaryRef)CFBridgingRetain(options));

    if (accessibilityEnabled) {
        printf("WE TRUSTED NOW\n");
    }
    
    //frontMostApp = getFrontMostApp();
    frontMostApp = AXUIElementCreateApplication(a);
    
    /* Get the front most window. We could also get an array of all windows
     * of this process and ask each window if it is front most, but that is
     * quite inefficient if we only need the front most window.
     */
    AXUIElementCopyAttributeValue(
                                  frontMostApp, kAXFocusedWindowAttribute, (CFTypeRef *)&frontMostWindow
                                  );
    
    /* Get the title of the window */
    AXUIElementCopyAttributeValue(
                                  frontMostWindow, kAXTitleAttribute, (CFTypeRef *)&windowTitle
                                  );
    
    /* Get the window size and position */
    AXUIElementCopyAttributeValue(
                                  frontMostWindow, kAXSizeAttribute, (CFTypeRef *)&temp
                                  );
    AXValueGetValue(temp, kAXValueCGSizeType, &windowSize);
    CFRelease(temp);
    
    AXUIElementCopyAttributeValue(
                                  frontMostWindow, kAXPositionAttribute, (CFTypeRef *)&temp
                                  );
    AXValueGetValue(temp, kAXValueCGPointType, &windowPosition);
    CFRelease(temp);
    
    /* Print everything */
    printf("\n");
    CFShow(windowTitle);
    printf(
           "Window is at (%f, %f) and has dimension of (%f, %f)\n",
           windowPosition.x,
           windowPosition.y,
           windowSize.width,
           windowSize.height
           );
    
    
    /* Move the window to the right by 25 pixels */
    windowPosition.x += 25;
    temp = AXValueCreate(kAXValueCGPointType, &windowPosition);
    AXUIElementSetAttributeValue(frontMostWindow, kAXPositionAttribute, temp);
    CFRelease(temp);
    
    windowSize.width += 400;
    windowSize.height += 400;
    temp = AXValueCreate(kAXValueCGSizeType, &windowSize);
    AXUIElementSetAttributeValue(frontMostWindow, kAXSizeAttribute, temp);
    CFRelease(temp);
    
    /* Clean up */
    CFRelease(frontMostWindow);
    CFRelease(frontMostApp);
    
    return 1;
}

@end

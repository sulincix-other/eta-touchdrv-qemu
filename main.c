#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <X11/Xlib.h>

#define serial "test"

int main(int argc, char **argv) {
    Display *display;
    XEvent xevent;
    Window window;
    XWindowAttributes window_attr;
    FILE *fd = fopen(serial, "w+");
    if(fd == NULL) {
        return -1;
    }


    if( (display = XOpenDisplay(NULL)) == NULL )
        return -1;


    window = DefaultRootWindow(display);
    XAllowEvents(display, AsyncBoth, CurrentTime);

    XGrabPointer(display,
                 window,
                 1,
                 PointerMotionMask | ButtonPressMask | ButtonReleaseMask ,
                 GrabModeAsync,
                 GrabModeAsync,
                 None,
                 None,
                 CurrentTime);

    
    XGetWindowAttributes(display, window, &window_attr);
    while(1) {
        // xxyyaaaabbbb
        // xx = event type
        // yy = event button
        // aaaa = x coordinate
        // bbbb = y coordinate
        int event = 0;
        fprintf(fd,"%1d%4d%4d\n", 0, window_attr.width, window_attr.height);
        fflush(fd);
        if(xevent.type == ButtonPress){
            puts("PRESS");
            event = 1;
        }
        if(xevent.type == ButtonRelease){
            puts("RELEASE");
            event = 2;
        }
        if(xevent.type == 6 ){
            puts("MOVE");
            event = 3;
        }
        if(event > 0){
            fprintf(fd,"%1d%4d%4d\n", event, xevent.xmotion.x_root, xevent.xmotion.y_root);
        }
        XNextEvent(display, &xevent);
        fflush(fd);
    }

    return 0;
}

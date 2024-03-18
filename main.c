#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <X11/Xlib.h>

#define serial "/dev/ttyS0"

int main(int argc, char **argv) {
    Display *display;
    XEvent xevent;
    Window window;
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
    
    while(1) {
        // xxyyaaaabbbb
        // xx = event type
        // yy = event button
        // aaaa = x coordinate
        // bbbb = y coordinate
        XNextEvent(display, &xevent);
        fprintf(fd, "%2d%2d%4d%4d\n",
            xevent.type,
            xevent.xbutton.button,
            xevent.xmotion.x_root,
            xevent.xmotion.y_root);
        fflush(fd);
    }

    return 0;
}

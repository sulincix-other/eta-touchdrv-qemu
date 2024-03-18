#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <linux/uinput.h>

#define EVENT_TYPE_SIZE 2
#define EVENT_BUTTON_SIZE 2
#define X_COORD_SIZE 4
#define Y_COORD_SIZE 4

static struct uinput_user_dev uidev;
static struct input_event ev;
static int fd;


int conv(int but){
    if(but == 1){
        return BTN_LEFT;
    }else if(but == 2){
        return BTN_MIDDLE;
    }else{
        return BTN_RIGHT;
    }
}

void simulateMouseEvent(int xtype, int button, int x, int y) {


    if (xtype == 6){
        printf("move: %d %d\n", x, y);
    struct input_event ev[2], ev_sync;
    memset(ev, 0, sizeof(ev));
    memset(&ev_sync, 0, sizeof(ev_sync));

    ev[0].type = EV_ABS;
    ev[0].code = ABS_X;
    ev[0].value = x;
    ev[1].type = EV_ABS;
    ev[1].code = ABS_Y;
    ev[1].value = y;


    int res_w = write(fd, ev, sizeof(ev));


    ev_sync.type = EV_SYN;
    ev_sync.value = 0;
    ev_sync.code = 0;
    int res_ev_sync = write(fd, &ev_sync, sizeof(ev_sync));
    }


    if (xtype == 5 || xtype == 4){
        // Simulate mouse event
        if (xtype == 5) {
            printf("press: %d %d \n", button, conv(button));
        } else {
            printf("release: %d %d \n", button, conv(button));
        }
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_KEY;
        ev.code = conv(button);
        ev.value = (xtype == 5);
        write(fd, &ev, sizeof(struct input_event));
        ev.type = EV_SYN;
        ev.code = SYN_REPORT;
        ev.value = 0;
        write(fd, &ev, sizeof(struct input_event));
    }

}



void uinput_init(){
 fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(fd, UI_SET_EVBIT, EV_ABS);
    ioctl(fd, UI_SET_ABSBIT, ABS_X);
    ioctl(fd, UI_SET_ABSBIT, ABS_Y);
    ioctl(fd, UI_SET_EVBIT, EV_REL);

    struct uinput_user_dev uidev;
    memset(&uidev,0,sizeof(uidev));
    snprintf(uidev.name,UINPUT_MAX_NAME_SIZE,"VirtualMouse");
    uidev.id.bustype = BUS_USB;
    uidev.id.version = 1;
    uidev.id.vendor = 0x1;
    uidev.id.product = 0x1;
    uidev.absmin[ABS_X] = 0;
    uidev.absmax[ABS_X] = 3200;
    uidev.absmin[ABS_Y] = 0;
    uidev.absmax[ABS_Y] = 900;

    write(fd, &uidev, sizeof(uidev));
    ioctl(fd, UI_DEV_CREATE);

    sleep(2);
}


void uinput_done(){
    // Close uinput device
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
}
int main() {
    uinput_init();
    FILE *fd = fopen("/dev/eta-mouse", "r");
    if (fd == NULL) {
        perror("Failed to open file");
        return 1;
    }

    int eventType, eventButton, xCoord, yCoord;
    while (fscanf(fd, "%2d%2d%4d%4d\n", &eventType, &eventButton, &xCoord, &yCoord) == 4) {
        // Simulate mouse event using parsed values
        printf("%2d:%2d:%4d:%4d\n", eventType, eventButton, xCoord, yCoord );
        simulateMouseEvent(eventType, eventButton, xCoord, yCoord);
    }

    uinput_done();
    return 0;
}


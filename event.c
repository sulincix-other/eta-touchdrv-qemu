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
        // Move mouse
        ev.type = EV_ABS;
        ev.code = ABS_X;
        ev.value = x;
        write(fd, &ev, sizeof(struct input_event));

        ev.type = EV_SYN;
        ev.code = SYN_REPORT;
        ev.value = 0;
        write(fd, &ev, sizeof(struct input_event));

        ev.type = EV_ABS;
        ev.code = ABS_Y;
        ev.value = y;
        write(fd, &ev, sizeof(struct input_event));

        ev.type = EV_SYN;
        ev.code = SYN_REPORT;
        ev.value = 0;
        write(fd, &ev, sizeof(struct input_event));
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
    // Open uinput device
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("Failed to open /dev/uinput");
        return;
    }
    // Configure uinput device
    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "amogus-mouse");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor = 0x1;
    uidev.id.product = 0x1;
    uidev.id.version = 1;

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(fd, UI_SET_EVBIT, EV_REL);
    ioctl(fd, UI_SET_EVBIT, EV_ABS);
    ioctl(fd, UI_SET_RELBIT, REL_X);
    ioctl(fd, UI_SET_RELBIT, REL_Y);
    ioctl(fd, UI_SET_RELBIT, ABS_X);
    ioctl(fd, UI_SET_RELBIT, ABS_Y);

    write(fd, &uidev, sizeof(uidev));
    ioctl(fd, UI_DEV_CREATE);
}

void uinput_done(){
    // Close uinput device
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
}
int main() {
    uinput_init();
    FILE *fd = fopen("test", "r");
    if (fd == NULL) {
        perror("Failed to open file");
        return 1;
    }

    int eventType, eventButton, xCoord, yCoord;
    while (fscanf(fd, "%2d%2d%4d%4d\n", &eventType, &eventButton, &xCoord, &yCoord) == 4) {
        // Simulate mouse event using parsed values
        printf("%2d%2d%4d%4d\n", eventType, eventButton, xCoord, yCoord );
        simulateMouseEvent(eventType, eventButton, xCoord, yCoord);
    }

    uinput_done();
    return 0;
}


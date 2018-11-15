#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

static unsigned int getTerminalWidth(){
    struct winsize w; //w.ws_row and w.ws_col
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

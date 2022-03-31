#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <stdlib.h>


static int end;
static void endme(int dummy) {
    end = 1;
}


int main(void) {

    int cmd0, count = 0, nextcount = 0;

    struct sched_param mysched;

    char wakeup;

    struct {
        char task, susres;
        int flags;
        long long time;
    } msg = {'S',};

    signal (SIGINT, endme);

    mysched.sched_priority = 99;

    if( sched_setscheduler( 0, SCHED_FIFO, &mysched ) == -1 ) {
        puts(" ERROR IN SETTING THE SCHEDULER UP");
        perror( "errno" );
        exit( 0 );
    }

    if ((cmd0 = open("/dev/rtf0", O_RDONLY)) < 0) {
        fprintf(stderr, "Error opening /dev/rtf0\n");
        exit(1);
    }

    while(!end) {
        read(cmd0, &msg, sizeof(msg));
        printf("> %c %c %x %lld\n", msg.task, msg.susres, msg.flags
               & 0x201, msg.time/1000000);
    }

}

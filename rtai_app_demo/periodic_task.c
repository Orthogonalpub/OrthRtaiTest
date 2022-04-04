
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
//#include <sys/time.h>
//#include <signal.h>
#include <fcntl.h>
#include <sched.h>
#include <rtai_lxrt.h>
//#include <rtai_sem.h>
//#include <rtai_msg.h>

static int thread0;

static void *fun0(void *arg) {

    RT_TASK *task;
    task = rt_task_init_schmod(nam2num("TASK0"), 0, 0, 0, SCHED_FIFO, 0xF);
    mlockall(MCL_CURRENT | MCL_FUTURE);

    //rt_make_hard_real_time();

    rt_printk("Hello World!\n");

    rt_make_soft_real_time();

    rt_task_delete(task);

    return 0;
}



int main(void) {
    RT_TASK *task;

    // make main thread LXRT soft realtime
    task = rt_task_init_schmod(nam2num("MYTASK"), 9, 0, 0, SCHED_FIFO, 0xF);
    mlockall(MCL_CURRENT | MCL_FUTURE);

    // start realtime timer and scheduler
    rt_set_oneshot_mode();
    start_rt_timer(0);

    // create a linux thread
    thread0 = rt_thread_create(fun0, NULL, 10000);

    // wait for end of program
    printf("TYPE <ENTER> TO TERMINATE\n");
    getchar();

    // cleanup stuff
    stop_rt_timer();
    return 0;
}

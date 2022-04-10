/* messqueue.c*/
/* ------------ headers ------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <linux/errno.h> /* error codes */
#include <signal.h>
#include <pthread.h>

#include <rtai_lxrt.h>
#include <rtai_mbx.h>
#include <rtai_msg.h>

/* ------------ globals ------------------------------------------- */

/* turn on(1) or off(0) debugging */
const int DEBUG=1;

// fix include warning
//#include <asm/system.h>
//#include <rtai_posix.h>
int pthread_cancel_rt(pthread_t thread);

// linux threads id's
static int thread1;
static int thread2;
// realtime task structures
static RT_TASK  *t1;
static RT_TASK  *t2;

// message queue
static MBX *mesgQueueId;
#define MAX_MESSAGES 100
#define MAX_MESSAGE_LENGTH 50

/* ------------ functions ------------------------------------------- */

void taskOne(void *arg)
{
    int retval;
    char message[] = "Received message from taskOne";

    /*  make this thread LXRT soft realtime */
    t1 = rt_task_init_schmod(nam2num("TASK1"), 0, 0, 0, SCHED_FIFO, 0xF);
    mlockall(MCL_CURRENT | MCL_FUTURE);

    // makes task hard real time (only when not developing/debugging)
    if ( !DEBUG )  rt_make_hard_real_time();

    rt_printk("Started taskOne\n");   
    /* send message */
    retval = rt_mbx_send(mesgQueueId, message, sizeof(message));
    if (0 != retval) {
        if (-EINVAL == retval) {
          rt_printk("mailbox is invalid\n");
        } else {
          /* unknown error */
          rt_printk("Unknown mailbox error\n");
        }
    }

}

void taskTwo(void *arg)
{
    int retval;
    char msgBuf[MAX_MESSAGE_LENGTH];

    /*  make this thread LXRT soft realtime */
    t2 = rt_task_init_schmod(nam2num("TASK2"), 0, 0, 0, SCHED_FIFO, 0xF);
    mlockall(MCL_CURRENT | MCL_FUTURE);

    // makes task hard real time (only when not developing/debugging)
    if ( !DEBUG )  rt_make_hard_real_time();

    rt_printk("Started taskTwo\n");

    /* receive message */
    retval = rt_mbx_receive_wp(mesgQueueId, msgBuf, 50);
    if (-EINVAL == retval) {
          rt_printk("mailbox is invalid\n");
    } else {
        rt_printk("message length=50-%d\n",retval);
        rt_printk("message : %s\n",msgBuf); 
    }
}

void cleanup(void)
{
    /* delete message queue */      
    rt_mbx_delete(mesgQueueId);

    // task end themselves -> not necesssary to delete them 
    return;
}

/* ------------ main ------------------------------------------- */

int main(void)
{   
    printf("Start of main\n");

    /*  make this thread LXRT soft realtime */
    rt_task_init_schmod(nam2num("TASK0"), 0, 0, 0, SCHED_FIFO, 0xF);
    mlockall(MCL_CURRENT | MCL_FUTURE);

    // set realtime timer to run in oneshot mode    
    rt_set_oneshot_mode();
    // start realtime timer and scheduler
    start_rt_timer(1);


    /* create message queue */    
    mesgQueueId = rt_typed_mbx_init (nam2num("MSGQUEUE"), MAX_MESSAGES, FIFO_Q);
    if (mesgQueueId  == 0) {
        printf("Error creating message queue\n");
        return 1;
    }

    // create the linux threads 
    thread1 = rt_thread_create(taskOne, NULL, 10000);
    thread2 = rt_thread_create(taskTwo, NULL, 10000);


    // wait for end of program
    printf("TYPE <ENTER> TO TERMINATE\n");
    getchar();

    // cleanup
    cleanup();

    printf("Finished\n");    
    return 0;
}

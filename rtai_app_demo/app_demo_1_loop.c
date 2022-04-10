#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <rtai_lxrt.h>
#include <sys/io.h>

#define TICK_TIME 1000000
#define CPUMAP 0x1

//static RT_TASK *main_Task;

static RT_TASK *loop_Task;
int keep_on_running = 1;
//static pthread_t main_thread;
static RTIME expected;
static RTIME sampling_interval;

static void *main_loop()
{

    if (!(loop_Task = rt_task_init_schmod(nam2num("RTAI01"), 2, 0, 0, SCHED_FIFO, CPUMAP))) 
    {
        printf("CANNOT INIT PERIODIC TASK\n");
        exit(1); 
    } 

    //mlockall(MCL_CURRENT | MCL_FUTURE);(源代码没有，…？)
    expected = rt_get_time() + 100*sampling_interval;
    rt_task_make_periodic(loop_Task, expected, sampling_interval); 

    rt_make_hard_real_time();

    while (keep_on_running)
    {
        //insert your main periodic loop here

       
        rt_printk("iiiiiiiiiiii CANNOT INIT MAIN TASK\n");


        rt_task_wait_period();//
        //set keep_on_running to 0 if you want to exit
    } 

    rt_task_delete(loop_Task);
    return 0;
}

int main(void)
{
    RT_TASK *Main_Task;
    if (!(Main_Task = rt_task_init_schmod(nam2num("MNTSK"), 0, 0, 0, SCHED_FIFO, 0xF))) 
    {
        printf("CANNOT INIT MAIN TASK\n");
        exit(1);
    } 

    //mlockall(MCL_CURRENT | MCL_FUTURE); 
    if (rt_is_hard_timer_running())
    {
        printf("Skip hard real_timer setting...\n");
        sampling_interval = nano2count(TICK_TIME);
    }
    else
    {
        printf("Starting real time timer...\n");
        rt_set_oneshot_mode();
        start_rt_timer(0);// 启动计时器 记得用完要关掉
    } 

    sampling_interval = nano2count(TICK_TIME);


    //pthread_create(&main_thread, NULL, main_loop, NULL);
    // create a linux thread 
    //thread0 = 
    rt_thread_create(main_loop, NULL, 10000);

    // wait for end of program
    printf("TYPE <ENTER> TO TERMINATE\n");
    getchar();

    // cleanup stuff
    stop_rt_timer();


    //while (keep_on_running) {
    //    sampling_interval = sampling_interval; //do nothing!
    //    printf("ccc  bbbbb  aaaaaaaaaaa\n");
   // }

    rt_task_delete(Main_Task);

    return 0;
}

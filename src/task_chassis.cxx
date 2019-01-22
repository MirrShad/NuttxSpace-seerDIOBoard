#include <nuttx/config.h>

#include <sys/ioctl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sched.h>
#include <errno.h>
#include <signal.h>

#include <sys/time.h>
#include "task_chassis.h"
#include "chassis.h"
#include "global_signal.h"

#define SIGVALUE_INT  42

extern "C"
{
  static void timer_expiration(int signo, siginfo_t *info, void *ucontext)
  {
    //printf("tash chassis timer_expiration\r\n");
    ChassisDevice::Instance()->queryOdoCmd();
  }

  int task_chassis(int argc, char *argv[])
  {
    printf("task chassis wait !!!!!!!!!!\r\n");
    ChassisDevice::Instance()->waitStart();
    printf("task chassis start !!!!!!!!!!\r\n");
    ChassisDevice::Instance()->doInit();

    //odo query timer init
    sigset_t           set;
    struct sigaction   act;
    struct sigaction   oact;//old act
    struct sigevent    notify;
    struct itimerspec  timer;
    timer_t            timerid;
    int                status;
    
    (void)sigemptyset(&set);
    (void)sigaddset(&set, CHASSIS_ODO_SIGNAL);
    status = sigprocmask(SIG_UNBLOCK, &set, NULL);
    if (status != OK)
    {
      printf("timer_test: ERROR sigprocmask failed, status=%d\n",
              status);
    }

    act.sa_sigaction = timer_expiration;
    act.sa_flags  = SA_SIGINFO;

    (void)sigfillset(&act.sa_mask);
    (void)sigdelset(&act.sa_mask, CHASSIS_ODO_SIGNAL);

    status = sigaction(CHASSIS_ODO_SIGNAL, &act, &oact);
    if (status != OK)
    {
      printf("timer_test: ERROR sigaction failed, status=%d\n" , status);
    }

  /* Create the POSIX timer */
    notify.sigev_notify            = SIGEV_SIGNAL;
    notify.sigev_signo             = CHASSIS_ODO_SIGNAL;
    notify.sigev_value.sival_int   = SIGVALUE_INT;
#ifdef CONFIG_SIG_EVTHREAD
    notify.sigev_notify_function   = NULL;
    notify.sigev_notify_attributes = NULL;
#endif

    status = timer_create(CLOCK_REALTIME, &notify, &timerid);
    if (status != OK)
    {
      printf("timer_test: timer_create failed, errno=%d\n", errno);
      //goto errorout;
    }

  /* Start the POSIX timer */
  //bug!!!!!!!i want 10ms interval but 10,000,000 won't get me such
    timer.it_value.tv_sec     = 0;
    timer.it_value.tv_nsec    = 10000000;//first expire time
    timer.it_interval.tv_sec  = 0;
    timer.it_interval.tv_nsec = 10000000;//interval time

    status = timer_settime(timerid, 0, &timer, NULL);
    if (status != OK)
    {
      printf("timer_test: timer_settime failed, errno=%d\n", errno);
      //goto errorout;
    }

    for(;;)
    {
      int tempCmd = ChassisDevice::Instance()->waitCmd();
      if(1 == tempCmd)
      {
        //printf("send speed cmd\r\n");
        ChassisDevice::Instance()->sendSpeedCmd();//send speed cmd
        ChassisDevice::Instance()->waitCmdRet(DRV_CMD_TARGET_SPEED);
      }
      else if(2 == tempCmd)
      {
        //printf("query odo cmd\r\n");
        ChassisDevice::Instance()->sendQueryCmd();//send query cmd
        ChassisDevice::Instance()->waitCmdRet(DRV_CMD_ACTURAL_POS);
        Mileage::Instance()->report();
        //report();
      }
      /*else if(3 == tempCmd)
        ;//query state cmd*/
    }
  }
}

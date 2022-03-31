/* Copyright (C) 2003 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@redhat.com>, 2003.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include <rtai_posix.h>

static pthread_cond_t cond; // = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mut; // = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;


static void *
tf (void *arg)
{
  pthread_setschedparam_np(0, SCHED_FIFO, 0, 0xF, PTHREAD_HARD_REAL_TIME_NP);
  int err = pthread_cond_wait (&cond, &mut);
  if (err == 0)
    {
      puts ("cond_wait did not fail");
      exit (1);
    }

  if (err != EPERM)
    {
      printf ("cond_wait didn't return EPERM but %d\n", err);
      exit (1);
    }


  /* Current time.  */
  struct timeval tv;
  (void) gettimeofday (&tv, NULL);
  /* +1000 seconds in correct format.  */
  struct timespec ts;
  TIMEVAL_TO_TIMESPEC (&tv, &ts);
  ts.tv_sec += 1000;

  err = pthread_cond_timedwait (&cond, &mut, &ts);
  if (err == 0)
    {
      puts ("cond_timedwait did not fail");
      exit (1);
    }

  if (err != EPERM)
    {
      printf ("cond_timedwait didn't return EPERM but %d\n", err);
      exit (1);
    }

  return (void *) 1l;
}


static int
do_test (void)
{
  pthread_t th;
  int err;

  printf ("&cond = %p\n&mut = %p\n", &cond, &mut);

  err = pthread_cond_wait (&cond, &mut);
  if (err == 0)
    {
      puts ("cond_wait did not fail");
      exit (1);
    }

  if (err != EPERM)
    {
      printf ("cond_wait didn't return EPERM but %d\n", err);
      exit (1);
    }


  /* Current time.  */
  struct timeval tv;
  (void) gettimeofday (&tv, NULL);
  /* +1000 seconds in correct format.  */
  struct timespec ts;
  TIMEVAL_TO_TIMESPEC (&tv, &ts);
  ts.tv_sec += 1000;

  err = pthread_cond_timedwait (&cond, &mut, &ts);
  if (err == 0)
    {
      puts ("cond_timedwait did not fail");
      exit (1);
    }

  if (err != EPERM)
    {
      printf ("cond_timedwait didn't return EPERM but %d\n", err);
      exit (1);
    }

  if (pthread_mutex_lock (&mut) != 0)
    {
      puts ("parent: mutex_lock failed");
      exit (1);
    }

  puts ("creating thread");

  if (pthread_create (&th, NULL, tf, NULL) != 0)
    {
      puts ("create failed");
      exit (1);
    }

  void *r;
  if (pthread_join (th, &r) != 0)
    {
      puts ("join failed");
      exit (1);
    }
  if (r != (void *) 1l)
    {
      puts ("thread has wrong return value");
      exit (1);
    }

  puts ("done");

  return 0;
}

int main(void)
{
	pthread_mutexattr_t mutattr;
        pthread_init_real_time_np("TASKA", 0, SCHED_FIFO, 0xF, PTHREAD_HARD_REAL_TIME);
        start_rt_timer(0);
        pthread_mutexattr_init(&mutattr);
	pthread_mutexattr_settype (&mutattr, PTHREAD_MUTEX_ERRORCHECK);
        pthread_mutex_init(&mut, NULL);
        pthread_cond_init(&cond, NULL);
        do_test();
        return 0;
}

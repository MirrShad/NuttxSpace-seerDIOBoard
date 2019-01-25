//***************************************************************************
// examples/cxxtest/main.cxx
//
//   Copyright (C) 2012, 2017 Gregory Nutt. All rights reserved.
//   Author: Qiang Yu, http://rgmp.sourceforge.net/wiki/index.php/Main_Page
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in
//    the documentation and/or other materials provided with the
//    distribution.
// 3. Neither the name NuttX nor the names of its contributors may be
//    used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
// OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
// AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//***************************************************************************

//***************************************************************************
// Included Files
//***************************************************************************

#include <nuttx/config.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <stdexcept>
#include <cassert>

#include <nuttx/init.h>

#include "platform/cxxinitialize.h"

#include "TasksPriority.h"
#include "task_heartled.h"
#include "task_commanddispatch.h"
#include "task_gyro.h"
#include "task_config.h"
#include "task_chassis.h"
#include "task_DIO.h"
#include "udp.h"
using namespace std;

//***************************************************************************
// Definitions
//***************************************************************************
// Configuration ************************************************************
// C++ initialization requires CXX initializer support

#if !defined(CONFIG_HAVE_CXX) || !defined(CONFIG_HAVE_CXXINITIALIZE)
#  warning Support for static initializers is NOT enabled
#  undef CONFIG_EXAMPLES_CXXTEST_CXXINITIALIZE
#endif

extern "C"
{
  int seer_main(int argc, char *argv[])
  {
    printf("Seer System Start\r\n");

    /* Call all C++ static constructors */
    up_cxxinitialize(); 
    
    //init udp net
    (void)udp_netinit();
 
    printf("Init heart beat task\r\n");
    int ret;
    ret = task_create("heart_beat_daemon", CONFIG_HEARTS_LEDS_PRIORITY,
                    2048, task_heartled,
                    NULL);
    if (ret < 0)
    {
      int errcode = errno;
      printf("leds_main: ERROR: Failed to start led_daemon: %d\n",
             errcode);
      return EXIT_FAILURE;
    }

    printf("Init config task\r\n");
    ret = task_create("config_daemon", CONFIG_CONFIG_PRIORITY,
                    2048, task_config,
                    NULL);
    if (ret < 0)
    {
      int errcode = errno;
      printf("leds_main: ERROR: Failed to start config_daemon: %d\n",
             errcode);
      return EXIT_FAILURE;
    }

    printf("Init command dispatch task\r\n");
    ret = task_create("commanddispatch_daemon", CONFIG_COMMAND_DISPATCH_PRIORITY,
                    2048, task_commanddispatch,
                    NULL);
    if (ret < 0)
    {
      int errcode = errno;
      printf("leds_main: ERROR: Failed to start commanddispatch_daemon: %d\n",
             errcode);
      return EXIT_FAILURE;
    }

    printf("Init gyro task\r\n");
    ret = task_create("gyro_daemon", CONFIG_GYRO_PRIORITY,
                    2048, task_gyro,
                    NULL);
    if (ret < 0)
    {
      int errcode = errno;
      printf("gyro_main: ERROR: Failed to start gyro_daemon: %d\n",
             errcode);
      return EXIT_FAILURE;
    }

    printf("Init chassis task\r\n");
    ret = task_create("chassis_daemon", CONFIG_CHASSIS_PRIORITY,
                    2048, task_chassis,
                    NULL);
    if (ret < 0)
    {
      int errcode = errno;
      printf("gyro_main: ERROR: Failed to start chassis_daemon: %d\n",
             errcode);
      return EXIT_FAILURE;
    }
    
    printf("Init DIO task\r\n");
    ret = task_create("DIO_daemon", CONFIG_DIO_PRIORITY,
                    2048, task_DIO,
                    NULL);
    if (ret < 0)
    {
      int errcode = errno;
      printf("gyro_main: ERROR: Failed to start DIO_daemon: %d\n",
             errcode);
      return EXIT_FAILURE;
    }

    return 0;
  }
}


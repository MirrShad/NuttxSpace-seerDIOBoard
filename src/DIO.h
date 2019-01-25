#ifndef _FW_DIO_H_
#define _FW_DIO_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "Singleton.h"
#include "Reporter.h"

class CDIODevice: public CReporter_base
{
public:
    void report();
};

typedef NormalSingleton<CDIODevice> DIO;

#endif

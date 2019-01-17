#ifndef __VERSION_H__
#define __VERSION_H__

#include "Singleton.h"
#include <stdio.h>

///////////////////////////////////////////////////////////////
/************************Version Define************************/
#define SDB_VERSION_MAJOR           2
#define SDB_VERSION_MINOR           0
#define SDB_VERSION_REVISION        0
/************************Version Define************************/
////////////////////////////////////////////////////////////////

#define SDB_BUILD_DATE      __DATE__
#define SDB_BUILD_TIME      __TIME__
#define SDB_STR_2(x)        #x
#define SDB_STR(x)          SDB_STR_2(x)
#define SDB_MAIN_VERSION    2.0.0

#define SDB_VERSION_LENGTH  100

class CVersion
{
public:
    CVersion(){resetStr();}

    int getMajor(){return SDB_VERSION_MAJOR;}
    int getMinor(){return SDB_VERSION_MINOR;}
    int getRevision(){return SDB_VERSION_REVISION;}

    const char* getMainVersion()
    {
        resetStr();
        sprintf(verStr,"%d.%d.%d",2,0,0);
        return verStr;
    }

    const char* getFullVersion()
    {
        resetStr();
        sprintf(verStr,"%s, built on %s-%s",'2.0.0',SDB_BUILD_DATE,
            SDB_BUILD_TIME);
        return verStr;
    }

private:
    void resetStr(){memset(verStr,0,SDB_VERSION_LENGTH);}

private:
    char verStr[SDB_VERSION_LENGTH];
};

typedef NormalSingleton<CVersion> Version;


#endif  //~__VERSION_H__

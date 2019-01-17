#include <nuttx/config.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <arpa/inet.h>

#include "task_config.h"
#include "udp.h"

#include "Communication.h"
#include "Gyro.h"
#include "version.h"

namespace{
    unsigned char _configBuffer[1024];
    uint8_t destip[4];
	uint16_t destport;
}

extern "C"
{
    void doConfig(CConfigReport &crd);

    int task_config(int argc, char *argv[])
    {
        struct sockaddr_in server5003;
        struct sockaddr_in client5003;
        in_addr_t tmpaddr;

        socklen_t addrlen;
        socklen_t recvlen;
        int sockfd;
        int nbytes;
        int optval;
    
        printf("Start config task\r\n");
        /* Create a new UDP socket */

        sockfd = socket(PF_INETX, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
            printf("server: socket failure: %d\n", errno);
            exit(1);
        }

    /* Set socket to reuse address */

        optval = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&optval, sizeof(int)) < 0)
        {
            printf("server: setsockopt SO_REUSEADDR failure: %d\n", errno);
            exit(1);
        }

        server5003.sin_family      = AF_INET;
        server5003.sin_port        = HTONS(5003);
        server5003.sin_addr.s_addr = HTONL(INADDR_ANY);

        addrlen                = sizeof(struct sockaddr_in);
    
        if (bind(sockfd, (struct sockaddr*)&server5003, addrlen) < 0)
        {
            printf("server: bind failure: %d\n", errno);
            exit(1);
        }

        for(;;)
        {
            recvlen = addrlen;
            nbytes = recvfrom(sockfd, _configBuffer, 1024, 0,
                        (struct sockaddr*)&client5003, &recvlen);
            if (nbytes < 0)
            {
                printf("config task server recv failed: %d\n", errno);
                close(sockfd);
                exit(-1);
            }

            int32_t int_ct;
            memcpy(&int_ct,_configBuffer,sizeof(int_ct));
            CConfigData::configType ct = (CConfigData::configType)int_ct;

            static CConfigReport crd;
            crd.resetDat();
		    crd.m_config_type = ct;

		    // do config.
		    doConfig(crd);
            nbytes = sendto(sockfd, (uint8_t*)&crd, crd.size(), 0, (sockaddr *)&client5003, recvlen);
        }

    }

    void doConfig(CConfigReport &crd)
    {
	    //config
	    CConfigData::configType head;
	    memcpy(&head, _configBuffer, sizeof(CConfigData::configType));
        
        
        switch (head)
        {
        case CConfigData::EDeviceDemandConfig:
            break;
        case CConfigData::EChassisStructConfig4cp:
        case CConfigData::EChassisStructConfig:
            break;
        case CConfigData::EChargeControl:
            {
                uint32_t tmp = 0;
                memcpy(&tmp,_configBuffer + sizeof(CConfigData::configType),sizeof(uint32_t));
                //DIO::Instance()->setChargeCmd(tmp);
            }
            break;
        case CConfigData::EKeepAlive:
            {
                uint32_t tmp = 0;
                memcpy(&tmp, _configBuffer + sizeof(CConfigData::configType), sizeof(uint32_t));
                if (0xFFFFFFFF == tmp)
                {
                    //aliveT.reset();

                    //Message::Instance()->clearErr(CODE_CONT_NET_DOWN);

                    if (false )
                    {
                        crd.m_config_report.data = 0;
                        printf("send EReboot");
                        //Message::Instance()->postMsg("send EReboot.");
                    } else {
                        crd.m_config_report.data = 1;
                    }
                }
            }
            break;

        case CConfigData::EGyroCalibration:
            {
                uint32_t tmp = 0;
                memcpy(&tmp, _configBuffer + sizeof(CConfigData::configType), sizeof(uint32_t));
                if (0xFFFFFFFF == tmp) //Calibrate once
                {
                    if(true == GyroDevice::Instance()->isOpen())
                        GyroDevice::Instance()->doCalibration();
                }
            }
            break;

        case CConfigData::EGyroAutoCaliSwitch:
            {

            }
            break;

        case CConfigData::EHostError:
            {
            /*
                uint32_t tmp = 0;
                memcpy(&tmp,_configBuffer + sizeof(CConfigData::configType), sizeof(uint32_t));
                if (0xFFFFFFFF == tmp)
                {
                    Message::Instance()->setErr(CODE_HOST_ERRO);
                    Console::Instance()->printf("Error in host\r\n");
                }
                else if(0 == tmp)
                {
                    Message::Instance()->clearErr(CODE_HOST_ERRO);
                    Console::Instance()->printf("Host error clear\r\n");
                }else
                {
                }
                */
            }
            break;
        case CConfigData::EChassisBrakeSwitch:
            {/*
                uint32_t tmp = 0;
                memcpy(&tmp,_configBuffer + sizeof(CConfigData::configType), sizeof(uint32_t));
                if (0xFFFFFFFF == tmp)
                {
                    GlobalControl::setChassisBrakeSwitchOn();
                }
                else if(0 == tmp)
                {
                    GlobalControl::setChassisBrakeSwitchOff();
                }
            */}
            break;

        case CConfigData::EInApplicationProgram:
            {
            /*    uint32_t tmp = 0;
                memcpy(&tmp,_configBuffer + sizeof(CConfigData::configType), sizeof(uint32_t));

                if (0xFFFFFFFF == tmp)
                {
                    pvf::write(pvf::VAR_BOOT_OPTI, BOOT_PARAM_BL);

    //				STMFLASH_write_bytes_noerase(pvf::read(pvf::VAR_BOOT_OPTI_ADDR), (uint8_t*)&boot_option, sizeof(uint32_t));
                    Console::Instance()->printf("Jump to bootloader...\r\n");
                    while(!Console::Instance()->isIdle());
                    NVIC_SystemReset();
                }*/
            }
            break;

        case CConfigData::EResetDIOBoard:
            {
                /*uint32_t tmp = 0;
                memcpy(&tmp,_configBuffer + sizeof(CConfigData::configType), sizeof(uint32_t));

                if (0xFFFFFFFF == tmp)
                {
                    Console::Instance()->printf("Get reset command!\r\n");
                    while(!Console::Instance()->isIdle());
                    NVIC_SystemReset();
                }*/
            }
            break;

        case CConfigData::ESetDO:
            {
                /*CDigitalOutput dOut;
                memcpy(&dOut,_configBuffer + sizeof(CConfigData::configType), sizeof(CDigitalOutput));
                GlobalData::RptDat.ioDat.setDO(dOut.pos,dOut.data);*/
                //Console::Instance()->printf("set DO[%d] = 0x%04X\r\n", dOut.pos, dOut.data);
            }
            break;

        case CConfigData::EGetUid:
        {
            uint32_t uid_addr = 0x1FFF7A10;
            memcpy(crd.m_config_report.m_buffer, (uint8_t*)uid_addr, 12);
            break;
        }

        case CConfigData::EUnlock:
        {
            /*if(0 == seer_uid_verify(_configBuffer + sizeof(CConfigData::configType)))
            {
                _isRsaPassed = true;
            }*/

            break;
        }

        case CConfigData::EGetMainVersion:
        {
            const char *p;
            p = Version::Instance()->getMainVersion();
            memcpy(crd.m_config_report.m_buffer, (void *)p, SDB_VERSION_LENGTH);
            break;
        }

        case CConfigData::EGetFullVersion:
        {
            const char *p;
            p = Version::Instance()->getFullVersion();
            memcpy(crd.m_config_report.m_buffer, (void *)p, SDB_VERSION_LENGTH);
            break;
        }

        case CConfigData::EPrintDebugInfo:
        {
            /*
            #if 1
            Console::Instance()->printf("Debug Info:\r\n");
            TaskManager::Instance()->printRunPool();
            #endif

            #if 1
            Console::Instance()->printf("Enabled report data:\r\n");

            if(GlobalData::RptDat.gyroDat.isEnable())
                Console::Instance()->printf("GyroDat Enabled\r\n");

            if(GlobalData::RptDat.odometerDat.isEnable())
                Console::Instance()->printf("odometerDat Enabled\r\n");

            if(GlobalData::RptDat.batteryDat.isEnable())
                Console::Instance()->printf("battDat Enabled\r\n");
            #endif

            #if 1
            Message::Instance()->printErrTable();
            #endif

            #if 1
            pvf::printField();
            #endif

            Console::Instance()->printf("========== IO state ==========\r\n");
            Console::Instance()->printf("DO = 0x%04X\r\n", GlobalData::RptDat.ioDat.getDO());
            Console::Instance()->printf("DI = 0x%04X\r\n", GlobalData::RptDat.ioDat.getDI());
            Console::Instance()->printf("Emergency Stop = %d\r\n", GlobalData::RptDat.IntBrdDat.emergencyStop);
            Console::Instance()->printf("Brake state = %d\r\n", GlobalData::RptDat.IntBrdDat.brkState);
            */
            break;
        }

        case CConfigData::ESetDefaultDO:
        {
            /*uint32_t tmp = 0;
            memcpy(&tmp,_configBuffer + sizeof(CConfigData::configType), sizeof(uint32_t));

            // TODO: need this ?
            const uint32_t EMG_DO_POS = (1ul << 16);
            tmp |= EMG_DO_POS;	//force emergency stop close

            if (tmp != pvf::read(pvf::VAR_DEFA_DO))
            {
                pvf::write(pvf::VAR_DEFA_DO, tmp);
                Console::Instance()->printf("set default DO state\r\n");
            }*/
            break;
        }

        case CConfigData::ELaunchEthCANTool:
        {
            /*uint32_t tmp = 0;
            memcpy(&tmp,_configBuffer + sizeof(CConfigData::configType), sizeof(uint32_t));
            */
            break;
        }

        case CConfigData::EPrintBaseTime:
        {
            /*uint64_t basetime = 0;
            uint64_t ntpTime = 0;
            BaseTimer::Instance()->getTime_ns(&basetime);
            ntpTime = basetime + SntpSynchr::Instance()->getNtpBaseOffset_ns();
            Console::Instance()->printf("BaseTimer: %lld ns, ntpTime: %lld ns\r\n", basetime, ntpTime );*/
            break;
        }

        case CConfigData::EGetMcuNtpTime:
        {
            /*uint64_t basetime = 0;
            uint64_t ntpTime = 0;
            BaseTimer::Instance()->getTime_ns(&basetime);
            ntpTime = basetime + SntpSynchr::Instance()->getNtpBaseOffset_ns();
            memcpy(crd.m_config_report.m_buffer, (void *)&ntpTime, sizeof(uint64_t));*/
            break;
        }
        case CConfigData::EGetGyroVersion:
        {
            /*memset(crd.m_config_report.m_buffer, 0, PACKET_CHAR_LENGTH);
            if (GyroDevice::Instance()->isOpen() && GlobalData::RptDat.gyroDat.isEnable())
            {
                GyroDevice::Instance()->getVersion_needMax20Char(crd.m_config_report.m_buffer);
            }
            else
            {
                sprintf(crd.m_config_report.m_buffer, "no gyro device");
            }*/
            break;
        }
        default:
            break;
        }

        
    }
}
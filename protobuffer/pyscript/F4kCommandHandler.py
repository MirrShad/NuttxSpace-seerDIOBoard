import socket
import struct
import sys
import os

F4kCommandPort = 15003
F4kAddr = ('192.168.192.4', F4kCommandPort)

def f4kernelcommand(packHead, arg):
    if arg == []:
        arg = [0xffffffff]
    msg = struct.pack('<' + str(len(arg) + 1) + 'I', packHead, *arg)
    so = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    so.settimeout(0.05)

    so.sendto(msg, F4kAddr)
    so.close()


def singleparam_querycommand(packHead, arg0):
    msg = struct.pack('<2I', packHead, arg0)
    so = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    so.settimeout(0.5)

    so.sendto(msg, F4kAddr)
    try:
        ret = so.recvfrom(1024)[0]
    except socket.timeout:
        ret = b''
    return ret
    so.close()


def noneparam_command(packHead):
    f4kernelcommand(packHead,[0xFFFFFFFF])


def noneparam_querycommand(packHead):
    return singleparam_querycommand(packHead, 0xffffffff)


def reset_commandhandler():
    f4kernelcommand(0x0000101E,[])


def empty_commandhandler():
    sys.stdout.flush()


def debuginfo_commandhandler():
    f4kernelcommand(0x0000100F,[])


def ntpcali_commandhandler():
    f4kernelcommand(0x00001013,[])



def uname_commandhandler():
    f4kernelcommand(0x00001032,[])

def gyroreset_commandhandler():
    f4kernelcommand(0x0000101A,[0x0A])

def gyrorepower_commandhandler():
    f4kernelcommand(0x0000101A,[0x03])


def uartconsole_commandhandler():
    packHead = 0x00001040
    f4kernelcommand(packHead,[0x01])


def gyrounlock_commandhandler():
    PACK_HEAD = 0x0000101A
    IAP_PACK_HEAD = 0x00009999
    GYRO_CMD_IAP_EN = 0x01
    GYRO_CMD_IAP_DIS = 0x02

    so = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    so.settimeout(0.05)

    so.sendto(struct.pack('<2I', PACK_HEAD, GYRO_CMD_IAP_EN), F4kAddr)
    so.sendto(struct.pack('<I8s', IAP_PACK_HEAD,
                          b'\xA5\x5A\x06\x07\xD6\x0E\xF1\xAA'), F4kAddr)
    so.sendto(struct.pack('<2I', PACK_HEAD, GYRO_CMD_IAP_DIS), F4kAddr)
    so.close()


def gyroversion_commandhandler():
    f4kernelcommand(0x0000101A,[0x09])

def ifconfig_commandhandler():
    import message_ipconfig_pb2
    destAddress = ('255.255.255.255', 15003)
    so = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    so.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

    so.settimeout(5)
    qMsg = struct.pack('<2I', 0x00001014, 0xffffffff)
    so.sendto(qMsg, destAddress)
    stmback = so.recvfrom(1024)[0]
    # (packHead, pbStr) = struct.unpack('<I'+ str(len(stmback) - 4) + 's', stmback)
    (packHead, pbStr) = struct.unpack(
        '<I' + str(len(stmback) - 4) + 's', stmback)

    # print(pbStr)
    pbMsg = message_ipconfig_pb2.Message_ipconfig()
    pbMsg.ParseFromString(pbStr)

    print('IP = %d.%d.%d.%d' %
          (pbMsg.ip[0], pbMsg.ip[1], pbMsg.ip[2], pbMsg.ip[3]))
    print('gateway = %d.%d.%d.%d' % (
        pbMsg.gateway[0], pbMsg.gateway[1], pbMsg.gateway[2], pbMsg.gateway[3]))
    print('subnet mask = %d.%d.%d.%d'
          % (pbMsg.subnet_mask[0], pbMsg.subnet_mask[1], pbMsg.subnet_mask[2], pbMsg.subnet_mask[3]))

    so.close()


def ododebug_commandhandler():
    f4kernelcommand(0x00001033,[0x01])

def odoreset_commandhandler():
    f4kernelcommand(0x00001033,[0x00])

def clear_commandhandler():
    os.system('cls')

def errorinfo_commandhandler():
    rawback = noneparam_querycommand(0x00001039)
    if(b'' == rawback):
        print('Query error info timeout, receive nothing')
        return

    msgback = struct.unpack('<' + str(len(rawback) >> 2) + 'I', rawback)

    from rbkerror import errortable
    errtype = ''
    for i in range(1, len(msgback)):
        if(msgback[i] > 56000):
            errtype = 'Notice'
        elif(msgback[i] > 54000):
            errtype = 'Warning'
        elif(msgback[i] > 52000):
            errtype = 'Error'
        else:
            errtype = 'Fatal'

        try:
            descript = errortable[str(msgback[i])]
        except KeyError:
            descript = 'Undefined'
        print('%s: %d - %s' % (errtype, msgback[i], descript))

def taskmanager_commandhandler():
    f4kernelcommand(0x0000104A,[0x01])

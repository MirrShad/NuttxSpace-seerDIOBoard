import CanFrame_pb2
import socket
import struct
import time
import os
import sys

if(len(sys.argv) == 2):
    import EthCanTransmitter

so = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
so.settimeout(2)


def launchEthCanTool():
    so.sendto(struct.pack('<2I', 0x1016, 0xffffffff), ('192.168.192.4', 15003))
    try:
        (data, remoteaddr) = so.recvfrom(1024)
    except socket.timeout:
        pass


def queryForFrame(num):
    fullMsg = struct.pack('<2I', 0x1019, num)
    so.sendto(fullMsg, ('192.168.192.4', 15003))


msgdict = {}
framerate = (0, 0)

launchEthCanTool()

while(True):
    time.sleep(0.01)

    cnt = 50
    queryForFrame(cnt)
    while(cnt > 0):
        cnt -= 1
        try:
            (data, remoteaddr) = so.recvfrom(1024)
        except socket.timeout:
            print('timeout')
            launchEthCanTool()
            break
        (msgId, pbdata) = struct.unpack('<I' + str(len(data) - 4) + 's', data)
        if(0x00001019 == msgId):
            frame = CanFrame_pb2.CanFrame()
            try:
                frame.ParseFromString(pbdata)
            except:
                print('pbdata parse error')
                print(data)
                os.system('pause')
                continue
            msgdict[frame.ID] = frame
        elif 0x00001041 == msgId:
            framerate = struct.unpack('<2I', pbdata)

    os.system('cls')
    sys.stdout.write('ID\tCH\tDLC\tDir\tTimestamp\tData\n')
    sys.stdout.write(
        '-----------------------------------------------------------------------------------\n')
    for key in msgdict.keys():
        # print(key)
        dirc = ''
        if(msgdict[key].Direction == 1):
            dirc = 'TX'
        else:
            dirc = 'RX'
        tmps = '0x%04X\t%d\t%d\t%s\t%d\t{ ' % (
            msgdict[key].ID, msgdict[key].Channel, msgdict[key].DLC, dirc, msgdict[key].Timestamp)
        for i in range(0, msgdict[key].DLC):
            tmps += ('0x%02X ' % msgdict[key].Data[i])
        tmps += '}\n'
        sys.stdout.write(tmps)
    sys.stdout.write(
        '-----------------------------------------------------------------------------------\n')
    sys.stdout.write("FrameRate:\tCH1: %d \t\tCH2: %d\n" %
                     (framerate[0], framerate[1]))
    sys.stdout.write(
        '-----------------------------------------------------------------------------------\n')

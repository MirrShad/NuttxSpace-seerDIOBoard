import socket
import struct
from time import sleep
import sys
import message_navigation_pb2

F4kCommandPort = 15003
F4kAddr = ('192.168.192.4', F4kCommandPort)
PACK_HEAD = 0x00001034

chassisCmd = message_navigation_pb2.Message_NavSpeed()
chassisCmd.x = 0.00
chassisCmd.y = 0
chassisCmd.rotate = 0.0
chassisCmd.steer_angle = 0.5

serializedChassisCmd = chassisCmd.SerializeToString()

so = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
so.settimeout(0.03)

msg = struct.pack('<I' + str(len(serializedChassisCmd)) + 's',
                  PACK_HEAD, serializedChassisCmd)

sendcnt = 100
while sendcnt > 0:
    try:
        sleep(0.03)
    except KeyboardInterrupt:
        so.close()
        exit()
    so.sendto(msg, F4kAddr)
    sendcnt -= 1
    print('.')
    sys.stdout.flush()

so.close()

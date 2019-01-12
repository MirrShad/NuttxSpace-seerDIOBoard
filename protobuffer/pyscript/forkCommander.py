import socket
import struct
from time import sleep
import sys

F4kCommandPort = 15003
F4kAddr = ('192.168.192.4', F4kCommandPort)
PACK_HEAD = 0x00001036


so = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
so.settimeout(0.03)
targetForkHeightPhy = 0.3
msg = struct.pack('<If', PACK_HEAD, targetForkHeightPhy)

so.sendto(msg, F4kAddr)
so.close()

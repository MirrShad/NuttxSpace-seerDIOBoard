import socket
import time

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
while(True):
    s.sendto(bytes([0x7F]), ('192.168.192.4', 5000))
    s.sendto(bytes([0x7F]), ('192.168.192.4', 6000))
    time.sleep(0.01)
s.close()

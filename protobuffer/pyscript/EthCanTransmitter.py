import CanFrame_pb2
import socket
import struct
import time
import os
import sys
import json

def genExampleMessage():
    print('Error detected please see example message: ex_msg.json')
    with open('ex_msg.json','w') as exmsg:
        exmsg.write('[\n{"id" : "1", "ext" : false, "data": "11 22 CC", "channel": 1}, \n' + \
            '{"id" : "5005", "ext" : true, "channel": 1, "data": "09 00 00 00 FF FF FF FF"}\n]')
        exmsg.close()

ethCanRemoteSendFrameHandler_head = 0x00001017
SeerDIO_addr = ('192.168.192.4', 15003)

if(len(sys.argv) != 2):
    print("Please input a .json CAN message")
    input()
    sys.exit()

msg_file = sys.argv[1]
tail = msg_file[-5:]
if (tail != ".json"):
    print("message extension is not .json")
    input()
    sys.exit()

errorFlag = False

try:
    with open(msg_file,'r') as f:
        json_CAN_msg = json.load(f)
        f.close()
except:
    genExampleMessage()
    print('Json decode error, press enter to exit...')
    input()
    quit()

so = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
so.settimeout(2)

for msg in json_CAN_msg:
    frame = CanFrame_pb2.CanFrame()
    try:
        frame.ID = int(msg["id"], base = 16)
    except KeyError:
        print("CAN ID input error")
        errorFlag = True
        break

    try:
        frame.Extended = msg["ext"]
    except KeyError:
        frame.Extended = False

    try:
        frame.Remote = msg["remote"]
    except KeyError:
        frame.Remote = False
    
    try:
        data_list = msg['data'].split(' ')
    except KeyError:
        print("CAN Data input error")
        errorFlag = True
        break

    if len(data_list) > 8:
        frame.DLC = 8
    else:
        frame.DLC = len(data_list)
    
    databytes = bytearray(frame.DLC)
    for i in range(0 , frame.DLC):
        databytes[i] = int(data_list[i], base = 16)
    frame.Data = bytes(databytes)

    try:
        frame.Channel = msg["channel"]
    except KeyError:
        print("CAN channel input error")
        errorFlag = True
        break

    serialized_pb_CAN_msg = frame.SerializeToString()
    seerDioCmder_msg = struct.pack('<I' + str(len(serialized_pb_CAN_msg)) + 's', ethCanRemoteSendFrameHandler_head, serialized_pb_CAN_msg)
    so.sendto(seerDioCmder_msg, SeerDIO_addr)
        
if(errorFlag):
    genExampleMessage()

print('Transmission finished, press enter to exit...')
input()
exit()
import socket
import struct
import signal
import threading
from time import sleep
from datetime import datetime
import sys
from prompt_toolkit import prompt
from prompt_toolkit.history import FileHistory
from prompt_toolkit.contrib.completers import WordCompleter
from F4kCommandHandler import *

global is_exit
global printFlag

is_exit = False
printFlag = True

input_mutex = threading.Lock()

consoleVersion = '1.0.1'


def exit_handler():
    global is_exit
    is_exit = True
    sys.exit()


commanddict = {
    '': empty_commandhandler,
    "exit": exit_handler,
    "reset": reset_commandhandler,
    'debuginfo': debuginfo_commandhandler,
    'ntpcali': ntpcali_commandhandler,
    'gyrounlock': gyrounlock_commandhandler,
    'gyroversion': gyroversion_commandhandler,
    'gyroreset': gyroreset_commandhandler,
    'gyrorepower': gyrorepower_commandhandler,
    'ifconfig': ifconfig_commandhandler,
    'uname': uname_commandhandler,
    'version': uname_commandhandler,
    'ododebug': ododebug_commandhandler,
    'odoreset': odoreset_commandhandler,
    'clear': clear_commandhandler,
    'errorinfo': errorinfo_commandhandler,
    'uartconsole': uartconsole_commandhandler,
    'taskmanager': taskmanager_commandhandler
}


def help_handler():
    print('All the available command is list as follow:')
    for key in commanddict:
        print(key)
    sys.stdout.flush()


commanddict['help'] = help_handler
commanddict['?'] = help_handler
cmdList = []
for key in commanddict:
    cmdList.append(key)

cmdCompleter = WordCompleter(cmdList)


def listenThreadFunc():
    global is_exit
    timestampflag = True
    local_addr = ('', 4999)
    so = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    so.settimeout(0.05)
    try:
        so.bind(local_addr)
    except OSError:
        print('There is already an udpConsole running, press enter to exit...')
        # input('Press enter to exit...')
        exit_handler()

    print('*** SeerDIOBoard udpConsole: V' + consoleVersion + ' ***\r\n')
    print('Local> Listening to local port %d' % local_addr[1])
    while True:
        try:
            (stmlog, addr) = so.recvfrom(1000)
        except socket.timeout:
            continue
        except NameError:
            quit()

        if printFlag is True:
            if timestampflag is True:
                sys.stdout.write('[' + datetime.strftime(datetime.now(), '%H:%M:%S.%f')[0:-3] + '] ')
                timestampflag = False

            try:
                logstr = stmlog.decode()
            except UnicodeDecodeError:
                print(stmlog)
            else:
                if logstr.find('\r\n') >= 0:
                    timestampflag = True

                sys.stdout.write(logstr)
                sys.stdout.flush()

        if(is_exit):
            so.close()
            print('\n\bLocal> Listen port close')
            quit()


listenThread = threading.Thread(target=listenThreadFunc)
listenThread.setDaemon(True)
listenThread.start()


while True:
    try:
        user_input = prompt('Local> ',
                            history=FileHistory('history.txt'),
                            completer=cmdCompleter,  patch_stdout=False)
    except KeyboardInterrupt:
        exit_handler()
    sys.stdout.flush()
    try:
        commanddict[user_input]()
    except KeyError:
        print('Unknow command, see \'help\'')
        sys.stdout.flush()

    if('' != user_input):
        printFlag = True
    else:
        printFlag = not printFlag

    if is_exit:
        exit_handler()

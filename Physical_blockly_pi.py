import time
import spidev

cs0 = 0
cs1 = 1

pi = spidev.SpiDev()
# spi.open(bus, device)
# cs1 is RFID
# cs0 is motor
cw = False
ccw = False
stop = False
tx = []
x = []
stop_time = time.time() + 200

def send_data(data):
    global tx
    pi.open(0, cs0)
    pi.mode = 0
    pi.max_speed_hz = 115200
    for i in range(len(data)):
        tx = pi.writebytes([ord(data[i])])  # send a single character
    pi.close()


def check_input():
    global x
    global stop
    global cw
    global ccw
    pi.open(0, cs1)
    pi.mode = 0
    pi.max_speed_hz = 115200
    x = pi.readbytes(1)
    if x == [1]:
        stop = True
        cw = False
        ccw = False
    if x == [2]:
        stop = False
        cw = True
        ccw = False
    if x == [3]:
        stop = False
        cw = False
        ccw = True
    pi.close()


while (time.time() < stop_time):
    data = ['\n','s','\r']
    send_data(data)
    check_input()
    #when stop is detected
    while stop and not cw and not ccw:
        print("stop is received")
        data = ['\n','s','\r']
        send_data(data)
        check_input()
    while cw and not stop and not ccw:
        print("cw is received")
        data = ['\n','c','w','\r']
        send_data(data)
        check_input()
    while ccw and not stop and not cw:
        print("ccw is received")
        data = ['\n','c','c','\r']
        send_data(data)
        check_input()

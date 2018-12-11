#! /usr/bin/env python
# coding:utf-8
# tcp_server

import socket
import threading
import csv
from time import sleep

bind_ip = '0.0.0.0'
bind_port = 50420
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((bind_ip, bind_port))
server.listen(5)
print(" [*] listen %s:%d" % (bind_ip, bind_port))
f = open("IoT_exercise1_received_data.csv", "w")
f.close()

def handle_client(client_socket):
    bufsize = 1024
    
    request = client_socket.recv(bufsize)
    request = request.decode('utf-8')
    list = request.split(",")
    if check(list):
        client_socket.send("OK")#ok
        sleep(5)
        csvfile = open("IoT_exercise1_received_data.csv", "a")
        writer = csv.writer(csvfile, lineterminator='\n')
        writer.writerow(list)
        csvfile.close()
    else:
        client_socket.send("ERROR")#error
        sleep(5)

    client_socket.close()

        #print('[*] recv: %s' % request)
    #client_socket.send("Hey Client!\n")
    #client_socket.close()

def check(list):
    if len(list) is not 4:
        return False
    try:
        ID = int(list[0])
        if ID < 0 or 3 < ID:
            print('ID wrong')
            return False
    except:
        return False
    try:
        timelist = list[1].split('T')
        day = timelist[0].split('-')
        time = timelist[1].split(':')
        for i in range(len(day)):
            day[i] = int(day[i])
        for i in range(len(time)):
            time[i] = int(time[i])
        if day[0] < 2018 or day[1] < 1 or day[1] > 12 or day[2] < 1 or day[2] > 31:
            print('time wrong1')
            return False
        if time[0] < 0 or time[0] > 24 or time[1] < 0 or time[1] > 60 or time[2] < 0 or time[2] > 60:
            print('time wrong2')
            return False
    except:
        return False
    try:
        lx = int(list[2])
    except:
        return False
    try:
        sensor = int(list[3])
        if sensor is not 0 and sensor is not 1:
            print('sensor wrong')
            return False
    except:
        return False
    print('sucess!\n')
    return True

while True:
    client, addr = server.accept()
    print('[*] connected from: %s:%d' % (addr[0], addr[1]))
    client_handler = threading.Thread(target = handle_client, args = (client,))
    client_handler.start()
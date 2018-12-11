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
    req = request.split(",")
    if len(req) == 4:
        client_socket.send("1")#ok
        csvfile = open("IoT_exercise1_received_data.csv", "a")
        writer = csv.writer(csvfile, lineterminator='\n')
        writer.writerow(req)
        csvfile.close()
    else:
        client_socket.send("2")#error

    sleep(5)
    client_socket.close()

        #print('[*] recv: %s' % request)
    #client_socket.send("Hey Client!\n")
    #client_socket.close()

while True:
    client, addr = server.accept()
    print('[*] connected from: %s:%d' % (addr[0], addr[1]))
    client_handler = threading.Thread(target = handle_client, args = (client,))
    client_handler.start()
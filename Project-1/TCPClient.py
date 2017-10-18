#!/usr/bin/env python
import socket

TCP_IP = '127.0.0.1'
TCP_PORT = 5005
BUFFER_SIZE = 1024

while 1:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((TCP_IP, TCP_PORT))
    MESSAGE = raw_input("Input lowercase sentence: ")
    s.send(MESSAGE)
    data = s.recv(BUFFER_SIZE)
    print "received data:", data
    s.close()

#https://wiki.python.org/moin/TcpCommunication

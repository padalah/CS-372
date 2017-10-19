#!/usr/bin/env python

import socket

TCP_IP = '127.0.0.1'
TCP_PORT = 5005
BUFFER_SIZE = 500

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))

while 1:
    sent = raw_input("Client: ")
    s.send(sent)
    if sent == "\quit": break
    received = s.recv(BUFFER_SIZE)
    if received == "\quit": break
    print "Server:", received
s.close()

#https://wiki.python.org/moin/TcpCommunication

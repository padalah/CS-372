#!/usr/bin/env python

import socket
import os

os.system('clear')
BUFFER_SIZE = 500

print "******* Server Info *******"
TCP_IP = raw_input("Hostname: ")
TCP_PORT = int(raw_input("Port Number: "))
print "****************************"
print "\n"

print "******* Client Setup *******"
handle = raw_input("User handle: ")
print "****************************"
print "\n"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print "=> Client has been created..."
s.connect((TCP_IP, TCP_PORT))
print "=> Connected to the server..."
print "=> You may begin messaging..."
print "\n"

print "-Messages-"
while 1:
    sent = raw_input(handle + ": ")
    s.send(handle + ": " + sent)
    if "\quit" in sent: break
    received = s.recv(BUFFER_SIZE)
    if "\quit" in received: break
    print received
s.close()

#https://wiki.python.org/moin/TcpCommunication

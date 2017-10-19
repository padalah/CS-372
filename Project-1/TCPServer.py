#!/usr/bin/env python

import socket
import os

BUFFER_SIZE = 500

while 1:
    os.system('clear')
    print "******* Server Setup *******"
    TCP_IP = raw_input("Hostname: ")
    TCP_PORT = int(raw_input("Port Number: "))
    handle = raw_input("User handle: ")
    print "****************************"
    print "\n"
    
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((TCP_IP, TCP_PORT))
    print "=> Server has been created..."
    s.listen(1)
    print "=> Listening for clients..."
    conn, addr = s.accept()
    print "=> Connected to a client..."
    print "       address:",addr[0]
    print "       port number:",addr[1]
    print "=> Awaiting message from client..."
    print "\n"
    print "-Messages-"


    while 1:
        received = conn.recv(BUFFER_SIZE)
        if "\quit" in received: break
        print received
        sent = raw_input(handle + ": ")
        conn.send(handle + ": " + sent)
        if "\quit" in sent: break
    conn.close()

# https://wiki.python.org/moin/TcpCommunication

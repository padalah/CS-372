#!/usr/bin/env python
import socket
import os
import sys

# check if the user input the correct number of arguments
if len(sys.argv) < 2:
    print "Expected format => python" + " " + sys.argv[0] + " " + "port"
    sys.exit()
# assign the portnumber
portnumber = int(sys.argv[1])
# create the socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# bind to the portnumber
s.bind(('',portnumber))
# listen for clients
s.listen(1)
# buffer size is 500 characters
buff = 500
# server's handle is "doug"
handle = "doug"
# loop until the user types CTRL + C (SIGINT) to end the program
while 1:
    # clear the command line
    os.system('clear')
    # waiting for message from client
    print "=> Awaiting message from client..."
    # waiting bool
    waiting = True
    # accept incoming client connection
    conn, addr = s.accept()
    # loop until client or server types \quit
    while 1:
        # received message from the client
        received = conn.recv(buff)
        # checks if \quit was in it
        if "\quit" in received: break
        # clears waiting message
        if waiting:
            os.system('clear')
            print "-Messages-"
        # no longer waiting
        waiting = False
        # prints it
        print received
        # gets the server's message
        sent = raw_input(handle + ": ")
        # sends it to the client
        conn.send(handle + ": " + sent)
        # checks if \quit was in it
        if "\quit" in sent: break
    conn.close()

# -RESOURCE-
# The resource below assisted me in completing this project.
# https://wiki.python.org/moin/TcpCommunication

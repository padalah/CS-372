#!/usr/bin/env python

'''
Jon-Eric Cook
CS-372
Project #1
This program demonstrates a client server setup. The server is run with python code 
and the client is run with C code. Both use the sockets API to talk to each through
a TCP connection. The server will be running, waiting for incoming connections. A 
client will connect to the server and send the first message. The server will then
respond. The client and server will go back and forth in this manner until either
the server or client send "\quit". When the server enters "\quit", it simply closes
the connection and then goes back to listening for the next client. When the client 
enters "quit", it will close the clients connection and end the program. The server 
needs to be terminated with a CTRL+C.
'''



import socket
import os
import sys



# This function checks if the user input the correct amount of arguments
# It will exit if they entered the wrong amount
def CheckArgCount():
    # check if the user input the correct number of arguments
    if len(sys.argv) < 2:
        print "Expected format => python" + " " + sys.argv[0] + " " + "port"
        sys.exit()



# This function starts up the server and returns the server variable
def StartUp():
    # assign the portnumber
    portnumber = int(sys.argv[1])
    # create the socket
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # bind to the portnumber
    s.bind(('',portnumber))
    # listen for clients
    s.listen(1)
    return s 



# This function receives a message from the client and returns it
def ReceiveMessage(c):
    return c.recv(500)



# This function sends a message to the client and returns it
def SendMessage(c):
    fromUser = raw_input("doug: ")
    # sends it to the client
    c.send("doug: " + fromUser)
    return fromUser



# Calls the function to the check the number of arguments
CheckArgCount()
# Calls the function to start up the server and stores the variable
server = StartUp()



# loops until the user types CTRL + C (SIGINT) to end the program
while 1:
    # clear the command line
    os.system('clear')
    # waiting for message from client
    print "=> Awaiting message from client..."
    # waiting bool
    waiting = True
    # accept incoming client connection
    conn, addr = server.accept()
    # loop until client or server types \quit
    while 1:
        # received message from the client
        received = ReceiveMessage(conn)
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
        sent = SendMessage(conn)
        # checks if \quit was in it
        if "\quit" in sent: break
    conn.close()

# -RESOURCE-
# The resource below assisted me in completing this project.
# https://wiki.python.org/moin/TcpCommunication
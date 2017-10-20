#!/usr/bin/env python

import socket
import os
import sys

portnumber = int(sys.argv[1])
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('',portnumber))
s.listen(1)
buff = 500
handle = "doug"

while 1:
    os.system('clear')
    conn, addr = s.accept()
    while 1:
        received = conn.recv(buff)
        if "\quit" in received: break
        print received
        sent = raw_input(handle + ": ")
        conn.send(handle + ": " + sent)
        if "\quit" in sent: break
    conn.close()

# https://wiki.python.org/moin/TcpCommunication

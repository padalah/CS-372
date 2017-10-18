#!/usr/bin/env python
import socket

TCP_IP = '127.0.0.1'
TCP_PORT = 5005
BUFFER_SIZE = 1024

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)

print 'The server is ready to receive'
while 1:
    conn, addr = s.accept()
    data = conn.recv(BUFFER_SIZE)
    capData = data.upper()
    conn.send(capData)
    conn.close()

#https://wiki.python.org/moin/TcpCommunication

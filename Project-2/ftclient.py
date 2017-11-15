import socket
import sys
import os
from os import path
from struct import *
from time import sleep

def print_argument_format():
    print("***** General argument format *****")
    print("python3 ftclient.py <server-host-name> <server-port-number> -l|-g [<file-name>] <data-port-number>\n")
    print("***** Acceptable argument formats *****")
    print("python3 ftclient.py flip1 1024->65535 -l 1024->65535")
    print("python3 ftclient.py flip1 1024->65535 -g file-name.txt 1024->65535\n")

def assign_arguments():
    global server_host_name
    global server_port_number
    global command
    global file_name
    global data_port_number
    server_host_name = sys.argv[1] + ".engr.oregonstate.edu"
    server_port_number = int(sys.argv[2])
    data_port_number = int(sys.argv[-1])
    command = sys.argv[3]
    if command == "-g":
        file_name = sys.argv[4]
    else:
        file_name = None

def validate_arguments():
    if len(sys.argv) not in (5,6):
        os.system('clear')
        print("ERROR: xpected 5 or 6 arguments\n")
        print_argument_format()
        exit(1)
    elif sys.argv[1] != "flip1":
        os.system('clear')
        print("ERROR: xpected server host name to be 'flip1'\n")
        print_argument_format()
        exit(1)
    elif int(sys.argv[2]) not in range(1024,65536):
        os.system('clear')
        print("ERROR: expected server port number to be in the range [1024, 65535]\n")
        print_argument_format()
        exit(1)
    elif int(sys.argv[-1]) not in range(1024,65536):
        os.system('clear')
        print("ERROR: expected data port number to be in the range [1024, 65535]\n")
        print_argument_format()
        exit(1)
    elif sys.argv[2] == sys.argv[-1]:
        os.system('clear')
        print("ERROR: expected the server and data port numbers to be different\n")
        print_argument_format()
        exit(1)
    elif sys.argv[3] not in ("-l","-g"):
        os.system('clear')
        print("ERROR: expected command to be either '-l' or '-g'\n")
        print_argument_format()
        exit(1)

def get_directory_contents(sock):
    ds = sock.recv(4)
    ds = unpack("I", ds)
    rec = str(sock.recv(ds[0]), encoding="UTF-8").split("\x00")
    for item in rec:
        print(item)

def get_file_from_server(sock, fn):
    buf = receive_message_from_server(sock)
    if path.isfile(fn):
        fn = fn.split(".")[0] + "_copy.txt"
    with open(fn, 'w') as f:
        f.write(buf)

def send_message_to_server(sock, message):
    output_message = bytes(message, encoding="UTF-8")
    sock.sendall(output_message)

def send_number_to_server(sock, number):
    output_number = pack('i', number)
    sock.send(output_number)

def send_request_to_server(sock, cmd, pn):
    send_message_to_server(sock, cmd + "\0")
    send_number_to_server(sock, pn)

def receive_message_from_server(sock):
    ds = sock.recv(4)
    ds = unpack("I", ds)
    return receive_entire_message_from_server(sock, ds[0])

def receive_entire_message_from_server(sock, n):
    rec = ""
    while len(rec) < n:
        packet = str(sock.recv(n - len(rec)), encoding="UTF-8")
        if not packet:
            return None
        rec += packet
    return rec

def make_contact_with_server(host_name, port_number):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host_name, port_number))
    return sock

if __name__ == '__main__':
    validate_arguments()
    assign_arguments()
    server = make_contact_with_server(server_host_name, server_port_number)
    send_request_to_server(server, command, data_port_number)
    if command == "-l":
        sleep(1)
        data_sock = make_contact_with_server(server_host_name, data_port_number)
        print("Receiving directory structure from {}: {}".format(sys.argv[1], data_port_number))
        get_directory_contents(data_sock)
        data_sock.close()
    if command == "-g":
        send_number_to_server(server, len(file_name))
        send_message_to_server(server, file_name + "\0")
        message_from_server = receive_message_from_server(server)
        if message_from_server == "FILE NOT FOUND":
            print("{}: {} says {}".format(sys.argv[1], server_port_number, message_from_server))
        elif message_from_server == "FILE FOUND":
            print("Receiving \"{}\" from {}: {}".format(file_name, sys.argv[1], data_port_number))
            sleep(1)
            data_sock = make_contact_with_server(sys.argv[1], data_port_number)
            get_file_from_server(data_sock, file_name)
            print("File transfer complete.")
            data_sock.close()
    server.close()

# -RESOURCES-
# The resources below assisted me in completing this project.
# https://stackoverflow.com/questions/82831/how-do-i-check-whether-a-file-exists-using-python
# https://github.com/gregmankes/cs372-project2/blob/master/ftclient.py
# https://stackoverflow.com/questions/6996603/how-to-delete-a-file-or-folder
# http://stackoverflow.com/questions/24196932/how-can-i-get-the-ip-address-of-eth0-in-python
# https://docs.python.org/2/howto/sockets.html
# https://docs.python.org/3.5/library/struct.html
# http://stackoverflow.com/questions/17667903/python-socket-receive-large-amount-of-data
# https://github.com/mustang25/CS372/blob/master/Project2/ftclient.py
# http://www.bogotobogo.com/python/python_network_programming_server_client_file_transfer.php
# https://pythongeekstuff.wordpress.com/2015/07/29/file-transfer-server-using-socket/

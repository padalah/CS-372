NOTE:
See resources at the bottom of chatserver.py and chatclient.c that assisted
me in completing this project. Also, they are below:
https://wiki.python.org/moin/TcpCommunication
http://www.linuxhowtos.org/C_C++/socket.htm

NOTE:
My project was successfully run on flip1.engr.oregonstate.edu.
I tested my program on my MacBook Pro and on the above school server.
I also tested my program on two different servers. I had my server running on flip1 and
my client running on flip2.

NOTE:
- How to run the chatserver.py program -
python chatserver.py <port#>

- How to compile chatclient.c -
gcc chatclient.c -o chatclient

- How to run the chatclient.c program -
./chatclient <server-hostname> <port#>

NOTE:
If a port number is in use, chatserver.py will abort and display the
following message "socket.error: [Errno 98] Address already in use".
Please run chatserver.py again and try another port number between 2000 - 65535.

NOTE:
Be sure that the port number passed to chatserver.py is the same as the port
number passed to ./chatclient


INSTRUCTIONS:
1) Run chatserver.py
    python chatserver.py <port#>

2) Compile chatclient.c
    gcc chatclient.c -o chatclient

3) Run chatclient.c
    ./chatclient <server-hostname> <port#>

4) On the client, enter a user handle

5) On the client, enter a message and press enter.

6) Client's message should be displayed on the server.

7) On the server, enter a message and press enter.

8) Server's message should be displayed on the client.

9) This continues until either the server or client enters "\quit".

10) When either the server or client enters "\quit", the client will terminate
    and the server will go back to waiting for a client to connect and send a
    message.

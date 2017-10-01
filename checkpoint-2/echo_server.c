/******************************************************************************
* echo_server.c                                                               *
*                                                                             *
* Description: Modification                                    *
*                                                                             *
* Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                         *
*          Wolf Richter <wolf@cs.cmu.edu>                                     *
*                                                                             *
*******************************************************************************/

#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>


#define ECHO_PORT 9999
#define BUF_SIZE 4096

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    printf("here 0");
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int close_socket(int sock)
{
    if (close(sock))
    {
        fprintf(stderr, "Failed closing socket.\n");
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[])
{   
   //TO-DO: open log file
  int sock, client_sock; // listening socket descriptor
  socklen_t cli_size;
  struct sockaddr_in addr, cli_addr; // client address
  char buf[4096]; // buffer for client data
  int nbytes;

  fd_set master;    // master file descriptor list
  fd_set read_fds;  // temp file descriptor list for select()
  int fdmax;        // maximum file descriptor number

  char remoteIP[INET6_ADDRSTRLEN];
  int i;


  fprintf(stdout, "----- Echo Server -----\n");
  
  /* all networked programs must create a socket */
  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
  {
      fprintf(stderr, "Failed creating socket.\n");
      return EXIT_FAILURE;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(ECHO_PORT);
  addr.sin_addr.s_addr = INADDR_ANY;

  //TO-do: logfile check failure
  FD_ZERO(&master);    // clear the master and temp sets
  FD_ZERO(&read_fds);

  /* servers bind sockets to ports---notify the OS they accept connections */
  if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)))
  {
      close_socket(sock);
      fprintf(stderr, "Failed binding socket.\n");
      return EXIT_FAILURE;
  }


  if (listen(sock, 5))
  {
      close_socket(sock);
      fprintf(stderr, "Error listening on socket.\n");
      return EXIT_FAILURE;
  }

  
  // add the listener to the master set
  FD_SET(sock, &master);

  // keep track of the biggest file descriptor
  fdmax = sock; // so far, it's this one
   printf("here 0");
  /* finally, loop waiting for input and then write it back */
  while (1)
  {
    read_fds = master; // copy it
    if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
        perror("select");
        exit(4);
    }

    // run through the existing connections looking for data to read
    for(i = 0; i <= fdmax; i++) {
        if (FD_ISSET(i, &read_fds)) { // we got one!!
            if (i == sock) {
                printf("here 1");
                // handle new connections
                cli_size = sizeof(cli_addr);
                client_sock = accept(sock,
                    (struct sockaddr *)&cli_addr,
                    &cli_size);
                printf("here 2");

                if (client_sock == -1) {
                    perror("accept");
                    printf("here 3");
                    //TO-DO: LOG
                } else {
                    FD_SET(client_sock, &master); // add to master set
                    printf("here 4");
                    if (client_sock > fdmax) {    // keep track of the max
                        fdmax = client_sock;

                    }
                    printf("selectserver: new connection from %s on "
                        "socket %d\n",
                        inet_ntop(cli_addr.sin_family,
                            get_in_addr((struct sockaddr*)&cli_addr),
                            remoteIP, INET6_ADDRSTRLEN),
                        client_sock);
                }
            } else {
                printf("here 5");
                // handle data from a client
                if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                    // got error or connection closed by client
                    if (nbytes == 0) {
                        // connection closed
                        printf("selectserver: socket %d hung up\n", i);
                        //TO-DO: LOG
                    } else {
                        perror("recv");
                        //TO-DO: LOG
                    }
                    close(i); // bye!
                    FD_CLR(i, &master); // remove from master set
                } else {
                    int status = send(i, buf, nbytes, 0);
                    if (status  == -1) {
                      perror("send");
                    }
                }
            } // END handle data from client
        } // END got new incoming connection
    } // END looping through file descriptors

  }

  close_socket(sock);

  return EXIT_SUCCESS;
}

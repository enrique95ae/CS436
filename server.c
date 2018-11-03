/*
 Web Server.
 
 Base code: provided by instructor.
 Modified and completed: Enrique Alonso Esposito
 Date: May 7th 2018
 Class: CS436 (Intro to Networking)
 Instructor: Professor Springer.
 
 Programmed in C.
 
 
 Purpose: to create a simple Web server capable of processing one request.
 The server will:
 1. Create a connection socket when contacted by the client. DONE
 2. Receive the request form the connection. DONE 
 3. Parse the request to determine the specific file being requested. NOT DONE
 4. Get the requeste3d file from the server's file system. DONE
 5. Create an response message consisting of the requested file preceded by header lines. DONE
 6. Send the response over the TCP connection to the requesting browser. DONE
 
 ----------------------------------------------------
 ----------------------------------------------------
 |                                                  |
 |             HOW TO USE IT:                       |
 |                                                  |
 ----------------------------------------------------
 ----------------------------------------------------
 |
 |  Modify the global variable "fileName"
 |  in order to change testing files. fileName is located
 |  in line 85 of the current code. I used a picture (.png) for testing,
 |  any file should work.
 |
 |  Compile: gcc server.c -lpthread -o serverExec
 |  Run server: ./serverExec 127.0.0.1 someport
 |  Run Client: ./clientExec 127.0.0.1 someport
 |
 |
 |  If client is run in a different host, the localhost IP will not work.
 |  The machine's IP in which the server is running should be used.
 |
 |
 |  If connection was established, socket created and file existes
 |  the file will be automatically sent.
 ----------------------------------------------------


NOT IMPLEMENTED YET:
In order for the server to send a file it has to be changed within the server's code.
Client does not request the file.
Parsing

*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/wait.h>
#include <pthread.h>

//DEFINES:
#define MAXLINE 256
#define bufferBytes 1024



//GLOBAL VARIABLES:

const int backlog = 4;
struct sockaddr_in cliaddr;

char fileName[] = "Networking.png";

  //FUNTION PROTOTYPES:
  void *client_request(void *arg);

int main(int argc, char *argv[])
{
  int listenfd;
  int connfd;
  pid_t   childpid;
  int clilen;
  struct  sockaddr_in servaddr;
  char sendBuffer[2000];
  int num;
  pthread_t tid; //Holds the thread's id number
    
  //Arguments checking.
  if (argc != 3) {
    printf("Usage: tcpserver <address> <port> \n");
    return EXIT_FAILURE;
  }
    
  //Retrieving socket
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
    
  //Socket checking
  if (listenfd == -1) {
    perror("socket error");
    return EXIT_FAILURE;
  }else{
    printf("socket success");
  }
    
  bzero(&servaddr, sizeof(servaddr));

  //assigning host's address and port
  servaddr.sin_family        = AF_INET;
  servaddr.sin_addr.s_addr   = inet_addr(argv[1]);
  servaddr.sin_port          = htons(atoi(argv[2]));
    
  //Binding address and port.
  if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
    perror("bind error");
    return EXIT_FAILURE;
        
  }
    
  //Initialize listen.
  if (listen(listenfd, backlog) == -1) {
    perror("listen error");
    return EXIT_FAILURE;
  }
    
  //Perpetually listening until connection request is received.
  while (1) {
    clilen = sizeof(cliaddr);
    if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0 )
      {
	if (errno == EINTR)
	  continue;
	else {
	  perror("aceppt error");
	  return EXIT_FAILURE;
	}
      }
        
    pthread_create(&tid, NULL, &client_request, (void *)&connfd);
  }
    
  close(connfd);
  return 0;
}

void *client_request(void *arg)
{
  int n;
  int connfd;
  connfd = *(int *)arg;
    
  write(connfd, fileName, MAXLINE);
    
  //opening the file to be sent
  FILE *file = fopen(fileName, "r");
  //checking if the file could be open.
  if(file == NULL)
    {
      perror("Error Opening the file");
      return EXIT_FAILURE;
    }
    

  while (1)
 {
   char fileBuffer[bufferBytes] = {0};
    n = fread(fileBuffer, 1, bufferBytes, file);
                
     if(n > 0)
      {
	write(connfd, fileBuffer, strlen(fileBuffer));
      }
  }
  close(connfd);
}

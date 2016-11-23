#define _POSIX_C_SOURCE 200809L

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "message.h"

void err(char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

// create a new socket
int createSocket()
{
  int soc;
  soc = socket(AF_INET, SOCK_STREAM, 0);
  return soc;
}

// connect given socket soc to server at "localhost" on port "2342"
void connectToTimeServer(int soc)
{
  if ( soc != -1 ) {

    // specify information about the hostname and port to use
    struct addrinfo hints;
    struct addrinfo *info;

    memset(&hints, 0, sizeof(hints)); // clear memory
    hints.ai_family = AF_INET;        // use IP as protocol
    hints.ai_socktype = SOCK_STREAM;  // use TCP as protocol
    hints.ai_flags = AI_PASSIVE;      // this side is used as the server

    // turn hostname and port information in addrinfo struct
    // information in hints is used as a hint to fill info
    int retval = getaddrinfo("localhost", "2342", &hints, &info);
    retval++; //nonsense

    connect(soc, info->ai_addr, info->ai_addrlen);

  }
}

// send time request message to the server
void sendTimeRequest(int soc, const time_request_t* timeRequest)
{
  if ( soc != -1 && timeRequest != NULL ) {
    send( soc, timeRequest, sizeof( time_request_t ), 0);
  }
}

// receive time respond message from the server and store it in timeRespond
void receiveTimeRespond(int soc, time_respond_t* timeRespond)
{
  if ( soc != -1 && timeRespond != NULL ) {
    int bytesReceived = recv(soc, (void *)timeRespond, sizeof(time_respond_t), 0);
    if (bytesReceived < 0) { close(soc); err("error while receiving"); }
  }
}

int main()
{
  int soc = createSocket();
  connectToTimeServer(soc);

  time_request_t timeRequest;
  time_respond_t timeRespond;

  timeRequest.timezone = +1; // CET
  sendTimeRequest(soc, &timeRequest);

  receiveTimeRespond(soc, &timeRespond);
  printf("Current time in Münster: %s\n", timeRespond.time);


  timeRequest.timezone = -5; // EST
  sendTimeRequest(soc, &timeRequest);

  receiveTimeRespond(soc, &timeRespond);
  printf("Current time in New York: %s\n", timeRespond.time);


  // send disconnect message
  timeRequest.timezone = 127; // magic number
  sendTimeRequest(soc, &timeRequest);

  close(soc);

  return 0;
}

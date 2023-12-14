#ifndef BLUEPRINTS
#define BLUEPRINTS

#include <algorithm>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <limits.h>
#include <netinet/ip.h>
#include <openssl/sha.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>

using namespace std;

class Tracker {
public:
  int id;
  string ip;
  int port;

  Tracker(int inputid, string inputip, int inputport) {
    id = inputid;
    ip = inputip;
    port = inputport;
  }
};

class Client {
public:
  int domain = AF_INET;
  u_long interface = INADDR_ANY;
  int type = SOCK_STREAM;
  int protocol = 0;

  int socket_status_id;
  int port;
  int request;

  Client(int inputport) {
    port = inputport;
    socket_status_id = socket(domain, type, protocol);
  }
  char *sendRequest(string trackerIp, int trackerport, string req,
                    int currpiecesize, int continuousDown) {
    cout << "preparing request \n";
    char *res = (char *)malloc(currpiecesize);
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = domain;
    serverAddress.sin_port = htons(trackerport);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    inet_pton(domain, trackerIp.c_str(), &serverAddress.sin_addr);
    int connectStatus =
        connect(socket_status_id, (struct sockaddr *)&serverAddress,
                sizeof(serverAddress));

    if (connectStatus == -1) {
      perror("Error in request connect \n");
    } else {
      cout << "Connected request successfully!" << endl;
    }

    int sendStatus = send(socket_status_id, req.c_str(), req.size(), 0);
    if (sendStatus == -1) {
      perror("Error in request send");
    } else {
      cout << " request Data sent successfully!" << endl;
    }

    if (!continuousDown) {
      int readStatus = read(socket_status_id, res, currpiecesize);
      if (readStatus == -1) {
        perror("Error in request response read");
      } else {
        cout << "request response Data read successfully!" << endl;
      }
      return res;
    }
    cout << "going to recieve size : " << currpiecesize << endl;

    int accumulatedSize = 0;
    char *res2 = (char *)malloc(currpiecesize);
    int i = 0;

    while (accumulatedSize < currpiecesize) {
      int remainingSize = currpiecesize - accumulatedSize;
      int currReadSize = read(socket_status_id, res2 + i, remainingSize);
      cout << "Reading small piece: " << currReadSize << " bytes" << endl;
      accumulatedSize += currReadSize;
      i += currReadSize;
    }
    return res2;
  }
};

class Server {
public:
  int domain;
  int type;
  int protocol;
  u_long interface;
  int backlog;

  struct sockaddr_in address;
  int socket_status_id;
  int bind_status;
  int listen_status;
  int port;

  Server(int inputport)
      : domain(AF_INET), type(SOCK_STREAM), protocol(0), interface(INADDR_ANY),
        backlog(20), port(inputport) {
    address.sin_family = AF_INET;
    address.sin_port = htons(inputport);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    // Create a socket
    socket_status_id = socket(domain, type, protocol);
    if (socket_status_id < 0) {
      cerr << "Error creating socket: " << strerror(errno) << endl;
    } else {
      cout << " socket created \n";
    }

    // Bind the socket
    bind_status =
        bind(socket_status_id, (struct sockaddr *)&address, sizeof(address));
    if (bind_status < 0) {
      cerr << "Error binding socket: " << strerror(errno) << endl;
    } else {
      cout << " socket bound \n";
    }

    // Listen on the socket
    listen_status = listen(socket_status_id, backlog);
    if (listen_status < 0) {
      cerr << "Error listening on socket: " << strerror(errno) << endl;
    } else {
      cout << " listening started \n";
    }
  }
};

#endif
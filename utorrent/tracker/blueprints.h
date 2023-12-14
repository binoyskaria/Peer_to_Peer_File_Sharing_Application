#ifndef BLUEPRINTS
#define BLUEPRINTS

#include <arpa/inet.h>
#include <iostream>
#include <netinet/ip.h>
#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include <unordered_map>
#include <set>
#include <openssl/sha.h>

using namespace std;


class User
{
public:
    string id;
    string pass;
    string clientip;
    int clientport;
    set<string> groupsasUser;
    set<string> groupsasAdmin;

    User(){

    }
    User(string inputid, string inputpass)
    {
        id = inputid;
        pass = inputpass;
    }
};

class Group
{
public:
  string groupname; 
    string adminofGroup;
    set<string> memberIds;
    set<string> joinrequestWaitlist;
    unordered_map<string,set<string>> seedersofeachFile;
    unordered_map<string,vector<string>> shaofeachFile;

    Group(){

    } 
    Group( string inputgroupname,string inputadminofGroup)  {
      groupname = inputgroupname;
      adminofGroup = inputadminofGroup;
    }
    
};


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
      : domain(AF_INET), type(SOCK_STREAM), protocol(0), interface(INADDR_ANY), backlog(20), port(inputport) {
    address.sin_family = AF_INET;
    address.sin_port = htons(inputport);
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    // Create a socket
    socket_status_id = socket(domain, type, protocol);
    if (socket_status_id < 0) {
      cerr << "Error creating socket: " << strerror(errno) << endl;
    }else{
      cout<<" socket created \n";
    }

    // Bind the socket
    bind_status = bind(socket_status_id, (struct sockaddr *)&address, sizeof(address));
    if (bind_status < 0) {
      cerr << "Error binding socket: " << strerror(errno) << endl;
    }else{
      cout<<" socket bound \n";
    }


    // Listen on the socket
    listen_status = listen(socket_status_id, backlog);
    if (listen_status < 0) {
      cerr << "Error listening on socket: " << strerror(errno) << endl;
    }else{
      cout<<" listening started \n";
    }

  }
};



#endif
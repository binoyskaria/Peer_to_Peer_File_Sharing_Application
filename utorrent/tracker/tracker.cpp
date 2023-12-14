
#include "general_utility_functions.h"

using namespace std;

unordered_map<string, User> usernameToDataMap;
unordered_map<string, Group> groupnametoDataMap;

void parallelAwaitClient(int socketStatus, Server listenServer);
void trackerAsServer(int port, string ip);

int main(int argc, char *argv[]) {


  if(strcmp(argv[1],"tracker_info.txt")!=0){
    cout<<"wrong text file";
    return 0;
  }
  if(strcmp(argv[2],"1")!=0){
    cout<<"wrong tracker number";
    return 0;
  }

  /*Fetch Client and Tracker Sockets
   * ****************************************************************************/
  /******************************************************************************/
  Tracker tracker1 = getTracker();

  thread mainServer(trackerAsServer, tracker1.port, tracker1.ip);
  mainServer.join();

  return 0;
}

void trackerAsServer(int port, string ip) {

  /*Recieve input , make thread to process it
  ********************************************************************/
  /************************************************************************/
  cout << "server creating connection on port:" << port << "\n";
  Server listenServer(port); // server on and listening on the provided port
  struct sockaddr *address;
  socklen_t addressLen;
  vector<thread> parallelClientHandlerThreads;
  while (true) {
    int socketStatus =
        accept(listenServer.socket_status_id, address, &addressLen);
    parallelClientHandlerThreads.emplace_back(
        [=] { parallelAwaitClient(socketStatus, listenServer); });
  }
}

/*Process recieved request
 * ************************************************************************************/
/**************************************************************************************/
void parallelAwaitClient(int socketStatus, Server listenServer) {

  cout << "connected , awaiting client command \n";

  char buffer[888];
  memset(buffer, 0, sizeof(buffer));
  int readStatus = read(socketStatus, buffer, sizeof(buffer));
  if (readStatus == -1) {
    perror("Error in read");

  } else if (readStatus == 0) {
    cout << "Connection closed by remote host." << endl;
  } else {
    cout << "Data read successfully!" << endl;
  }

  string incomingReq(buffer);

  vector<string> incomingReqvec = splitwordsbyspace(incomingReq);
  cout << "splitted recieved sentence:  ";
  for (auto i : incomingReqvec) {
    cout << i << " ";
  }
  cout << endl;

  if (!incomingReqvec.empty() && incomingReqvec[0] == "test") {
    cout << "test success \n";
    cout << "user to Data Map \n ==================\n";
    for (auto i : usernameToDataMap) {
      cout << "username: " << i.first << " username: " << i.second.id
           << " password: " << i.second.pass
           << " clientip: " << i.second.clientip << " clientport"
           << i.second.clientport << endl
           << endl;

      cout << "groups as admin \n ----------------- \n";
      for (auto j : i.second.groupsasAdmin) {
        cout << j << " ";
      }
      cout << endl;

      cout << "groups as user \n -------------------- \n";
      for (auto j : i.second.groupsasUser) {
        cout << j << " ";
      }
      cout << endl;
    }

    cout << "group to Data Map \n ==================\n";
    for (auto i : groupnametoDataMap) {
      cout << "groupname: " << i.first << " groupname: " << i.second.groupname
           << " admin of group: " << i.second.adminofGroup << endl;
      cout << "members \n ----------------------\n";
      for (auto j : i.second.memberIds) {
        cout << j << " ";
      }
      cout << endl;
      cout << "join Requests \n ----------------------\n";
      for (auto j : i.second.joinrequestWaitlist) {
        cout << j << " ";
      }
      cout << endl;
      cout << "seeder of each file \n ----------------------\n";
      for (auto j : i.second.seedersofeachFile) {
        cout << j.first << " ";
        for (auto k : j.second) {
          cout << k << " ";
        }
        cout << endl;
      }
      cout << endl;
      cout << "sha of each file \n ----------------------\n";
      for (auto j : i.second.shaofeachFile) {
        cout << j.first << " ";
        for (auto k : j.second) {
          cout << k << " ";
        }
        cout << endl;
      }
      cout << endl;
    }
    send(socketStatus, "success", 21, 0);
  }

  else if (!incomingReqvec.empty() && incomingReqvec[0] == "create_user") {
    string userid = incomingReqvec[1];
    string pass = incomingReqvec[2];
    if (usernameToDataMap.find(userid) == usernameToDataMap.end()) {
      User tempuser(userid, pass);
      usernameToDataMap[userid] = tempuser;
      send(socketStatus, "success", 21, 0);
      cout << "this user creation success:  ";
      cout << usernameToDataMap[userid].id << endl;
    } else {
      send(socketStatus, "failure user already exists", 21, 0);
      cout << "user alreaady exists \n";
    }
  }

  else if (!incomingReqvec.empty() && incomingReqvec[0] == "login") {
    string incominguserid = incomingReqvec[1];
    string incomingpass = incomingReqvec[2];
    string incomingclientip = incomingReqvec[3];
    int incomingclientport = stoi(incomingReqvec[4]);

    if (usernameToDataMap.find(incominguserid) != usernameToDataMap.end() &&
        usernameToDataMap[incominguserid].pass == incomingpass) {

      usernameToDataMap[incominguserid].clientip = incomingclientip;
      usernameToDataMap[incominguserid].clientport = incomingclientport;
      send(socketStatus, "success", 21, 0);
      cout << "this user logged in: ";
      cout << usernameToDataMap[incominguserid].id << endl;
    } else {
      send(socketStatus, "failure user does not exist", 21, 0);
      cout << "user does not exist \n";
    }
  }

  else if (!incomingReqvec.empty() && incomingReqvec[0] == "create_group") {
    string incominggroupname = incomingReqvec[1];
    string incomingadmin = incomingReqvec[2];

    if (groupnametoDataMap.find(incominggroupname) ==
        groupnametoDataMap.end()) {
      Group temppgroup(incomingReqvec[1], incomingReqvec[2]);

      groupnametoDataMap[incominggroupname] = temppgroup;
      groupnametoDataMap[incominggroupname].memberIds.insert(incomingadmin);

      send(socketStatus, "success", 21, 0);
      cout << "this group created: ";
      cout << groupnametoDataMap[incominggroupname].groupname << endl;
    } else {
      send(socketStatus, "failure group already exist", 21, 0);
      cout << "group already exist \n";
    }
  }

  else if (!incomingReqvec.empty() && incomingReqvec[0] == "list_groups") {

    string responseString;
    for (auto i : groupnametoDataMap) {
      responseString += i.second.groupname + " ";
    }

    send(socketStatus, responseString.c_str(), 21, 0);
    cout << "group list sent \n";
  } else if (!incomingReqvec.empty() && incomingReqvec[0] == "join_group") {

    string incomingGroup = incomingReqvec[1];
    string incomingUser = incomingReqvec[2];

    if (groupnametoDataMap.find(incomingGroup) != groupnametoDataMap.end() &&
        usernameToDataMap.find(incomingUser) != usernameToDataMap.end()) {
      groupnametoDataMap[incomingGroup].joinrequestWaitlist.insert(
          incomingUser);
      send(socketStatus, "success", 21, 0);
      cout << "join request recieved \n";
    } else {
      send(socketStatus, "failure : please double check details", 21, 0);
      cout << "join request failed, please double check data \n";
    }

  }

  else if (!incomingReqvec.empty() && incomingReqvec[0] == "accept_request") {

    string incomingGroup = incomingReqvec[1];
    string incomingUser = incomingReqvec[2];
    string incomingcheckAdminString = incomingReqvec[3];

    if (groupnametoDataMap.find(incomingGroup) != groupnametoDataMap.end() &&
        usernameToDataMap.find(incomingUser) != usernameToDataMap.end() &&
        groupnametoDataMap[incomingGroup].joinrequestWaitlist.find(
            incomingUser) !=
            groupnametoDataMap[incomingGroup].joinrequestWaitlist.end() &&
        groupnametoDataMap[incomingGroup].adminofGroup ==
            incomingcheckAdminString) {
      groupnametoDataMap[incomingGroup].memberIds.insert(incomingUser);
      groupnametoDataMap[incomingGroup].joinrequestWaitlist.erase(incomingUser);
      send(socketStatus, "success", 21, 0);
      cout << "join request recieved \n";

    } else {
      send(socketStatus, "failure : please double check details", 21, 0);
      cout << "join request failed, please double check data \n";
    }
  }

  else if (!incomingReqvec.empty() && incomingReqvec[0] == "leave_group") {

    string incomingGroup = incomingReqvec[1];
    string incomingUser = incomingReqvec[2];
    string incomingcheckSessionString = incomingReqvec[3];

    if (groupnametoDataMap.find(incomingGroup) != groupnametoDataMap.end() &&
        usernameToDataMap.find(incomingUser) != usernameToDataMap.end() &&
        groupnametoDataMap[incomingGroup].memberIds.find(incomingUser) !=
            groupnametoDataMap[incomingGroup].memberIds.end()) {

      groupnametoDataMap[incomingGroup].memberIds.erase(incomingUser);

      if (groupnametoDataMap[incomingGroup].adminofGroup == incomingUser) {
        if (!groupnametoDataMap[incomingGroup].memberIds.empty()) {
          groupnametoDataMap[incomingGroup].adminofGroup =
              *groupnametoDataMap[incomingGroup].memberIds.begin();
        }
      }
      if (groupnametoDataMap[incomingGroup].memberIds.empty()) {
        groupnametoDataMap.erase(incomingGroup);
      }

      send(socketStatus, "success", 21, 0);
      cout << "user deleted from group \n";

    } else {
      send(socketStatus, "failure : please double check details", 21, 0);
      cout << "user deletion failed, please double check data \n";
    }
  }

  else if (!incomingReqvec.empty() && incomingReqvec[0] == "list_requests") {

    string incomingGroup = incomingReqvec[1];
    string incomingUser = incomingReqvec[2];

    if (groupnametoDataMap.find(incomingGroup) != groupnametoDataMap.end() &&
        usernameToDataMap.find(incomingUser) != usernameToDataMap.end() &&
        groupnametoDataMap[incomingGroup].adminofGroup == incomingUser) {
      string response = "";
      for (auto i : groupnametoDataMap[incomingGroup].joinrequestWaitlist) {
        response += " " + i;
      }

      send(socketStatus, response.c_str(), 21, 0);
      cout << "joinrequest list sent \n";

    } else {
      send(socketStatus, "failure : please double check details", 21, 0);
      cout << "list sent failed, please double check data \n";
    }
  }

  else if (!incomingReqvec.empty() && incomingReqvec[0] == "upload_file") {
    string incomingFilePath = incomingReqvec[1];
    string incomingGroup = incomingReqvec[2];
    string incomingUser = incomingReqvec[3];

    if (groupnametoDataMap.find(incomingGroup) != groupnametoDataMap.end() &&
        usernameToDataMap.find(incomingUser) != usernameToDataMap.end()) {

      groupnametoDataMap[incomingGroup]
          .seedersofeachFile[incomingFilePath]
          .insert(incomingUser);

      send(socketStatus, "success", 21, 0);
      cout << "user added as seeder for file \n";

    } else {
      send(socketStatus, "failure : please double check details", 21, 0);
      cout << "list sent failed, please double check data \n";
    }
  }

  else if (!incomingReqvec.empty() && incomingReqvec[0] == "uploadsha") {

    string incominggroupid = incomingReqvec[1];
    string incomingfilepath = incomingReqvec[2];
    string incomingshapiece = incomingReqvec[3];
    cout << "recieving piece" << incomingshapiece << endl;
    groupnametoDataMap[incominggroupid].shaofeachFile[incomingfilepath].push_back(
        incomingshapiece);
    send(socketStatus, "success", 21, 0);
  }

  else if (!incomingReqvec.empty() && incomingReqvec[0] == "list_files") {

    string incominggroupid = incomingReqvec[1];
    if (groupnametoDataMap.find(incominggroupid) != groupnametoDataMap.end()) {
      unordered_map<string, vector<string>> i =
          groupnametoDataMap[incominggroupid].shaofeachFile;
      cout << "sha of each file \n ----------------------\n";
      for (auto j : i) {
        cout << j.first << " ";
        for (auto k : j.second) {
          cout << k << " ";
        }
        cout << endl;
      }
      cout << endl;

      send(socketStatus, "success", 21, 0);
    }

  }

  else if (!incomingReqvec.empty() && incomingReqvec[0] == "download_file") {

    string incominggroupid = incomingReqvec[1];
    string incomingfilepath = incomingReqvec[2];
    string incomingdestfilepath = incomingReqvec[3];
    string incominguser = incomingReqvec[4];

    if (groupnametoDataMap.find(incominggroupid) != groupnametoDataMap.end() &&
        groupnametoDataMap[incominggroupid].seedersofeachFile.find(
            incomingfilepath) !=
            groupnametoDataMap[incominggroupid].seedersofeachFile.end()

    ) {

      string response = "";
      for (auto i : groupnametoDataMap[incominggroupid]
                        .seedersofeachFile[incomingfilepath]) {

          response+=i+"-";
          response+=usernameToDataMap[i].clientip+"-";
          response+=to_string(usernameToDataMap[i].clientport)+" ";
      }
      send(socketStatus, response.c_str(), 21, 0);
    } else {
      send(socketStatus, "failure", 21, 0);
      cout << "download initialization failed";
    }
  }

  else if (!incomingReqvec.empty() && incomingReqvec[0] == "newseeder") {
    string incominggroupname = incomingReqvec[1];
    string incomingfilepath = incomingReqvec[2];
    string incominguser = incomingReqvec[3];
    if (groupnametoDataMap.find(incominggroupname) != groupnametoDataMap.end()) {
      
      groupnametoDataMap[incomingfilepath].seedersofeachFile[incomingfilepath].insert(incominguser);

      send(socketStatus, "success", 21, 0);
      cout << "success: user inserted  ";
      
    } else {
      send(socketStatus, "failure please check data", 21, 0);
      cout << "failure please check data \n";
    }
  }

  else if (!incomingReqvec.empty() && incomingReqvec[0] == "getshaoffile") {
    string incominggroupname = incomingReqvec[1];
    string incomingfilepath = incomingReqvec[2];
    
    if (groupnametoDataMap.find(incominggroupname) != groupnametoDataMap.end()) {
      
     vector<string> returnsha = groupnametoDataMap[incominggroupname].shaofeachFile[incomingfilepath];
     string response= "";
     for(auto i: returnsha){
      response+=i+" ";
     }

      send(socketStatus, response.c_str(), response.size(), 0);
      cout << "success: user inserted  ";
      
    } else {
      send(socketStatus, "failure", 21, 0);
      cout << "failure please check data \n";
    }
  }
}

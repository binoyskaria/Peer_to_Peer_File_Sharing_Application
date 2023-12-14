#include "blueprints.h"
#include "general_utility_functions.h"

//524288
#define pieceblock 524288
using namespace std;
void clientAsClient(string request, int clientPort, string clientIp,
                    Tracker tracker1);

void clientAsServer(int port);

void bitmapfromeachuser(
    string incominggroupname, string clientid, string clientip, int clientport,
    string incomingfilepath,

    unordered_map<string, pair<string, int>> &usertocurrentfileBitmapV2,
    sem_t &mutex);

void piecedownload(string fileName, string peerIp, int peerPort, int chunkNo,
                   int currChunkSize, int fd);

unordered_map<string, pair<vector<bool>, int>> filetobitmap;

unordered_map<string, pair<string, int>> usertocurrentfileBitmapV2;
vector<pair<int, pair<int, vector<string>>>> pieceToNoofpieceNseederList;
vector<string> seedersofcurrentfile;

string sessionUserId = "";

int main(int argc, char *argv[]) {

  
  string clientIp;
  string clientportstr;
  int clientPort;

  string whole = argv[1];
  splitString(whole, clientIp, clientportstr);
  clientPort = stoi(clientportstr);


  /*Fetch Client and Tracker Sockets
   ******************************************************************************/
  /******************************************************************************/
  
  struct sockaddr_in servaddr = getDefaultServerStructure();
  Tracker tracker1 = getTracker();

  thread serverThread(clientAsServer, clientPort);
  vector<thread> parallelClientThreads;
  while (true) {

    /*Take input and make it a thread
     *******************************************************************************/
    /*******************************************************************************/
    cout << "awaiting request \n";
    string request;
    getline(cin, request);
    int socketStatus = socket(AF_INET, SOCK_STREAM, 0);
    int connection_status =
        connect(socketStatus, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (connection_status == -1) {
      perror("Error in main connect \n");
    } else {
      cout << "Connected main successfully!" << endl;
    }
    if (request != "")
      parallelClientThreads.emplace_back(clientAsClient, request, clientPort,
                                         clientIp, tracker1);
  }
}

/*Process input request
 *****************************************************************************************/
/*****************************************************************************************/
void clientAsClient(string request, int clientPort, string clientIp,
                    Tracker tracker1) {
  vector<string> requestvec = splitwordsbyspace(request);
  if (!requestvec.empty() && requestvec[0] == "test") {
    Client client1(clientPort);
    char *res = client1.sendRequest(tracker1.ip, tracker1.port, request,18888,0);

    cout << "bitmap of each file (filetobitmap) \n ===================\n";
    for (auto i : filetobitmap) {
      cout << i.first << " ";
      for (auto j : i.second.first) {
        cout << j << " ";
      }
      cout << i.second.second;
      cout << endl;
    }
    cout << endl;

    string response(res);
    if (response == "success") {
      cout << "test success \n";
    } else {
      cout << response << endl;
    }
    close(client1.socket_status_id);
  } else if (!requestvec.empty() && requestvec[0] == "create_user") {
    Client client1(clientPort);
    char *res = client1.sendRequest(tracker1.ip, tracker1.port, request,18888,0);

    string response(res);
    if (response == "success") {
      cout << "user created \n";
    } else {
      cout << response << endl;
    }
    close(client1.socket_status_id);
  } else if (!requestvec.empty() && requestvec[0] == "login") {
    Client client1(clientPort);
    request += " " + clientIp + " " + to_string(clientPort);
    char *res = client1.sendRequest(tracker1.ip, tracker1.port, request,18888,0);

    string response(res);
    if (response == "success") {
      cout << "user created \n";
      sessionUserId = requestvec[1];
    } else {
      cout << response << endl;
    }
    close(client1.socket_status_id);
  }

  else if (!requestvec.empty() && requestvec[0] == "create_group") {
    if (sessionUserId == "") {
      cout << "No User logged in, PLease Login \n";
      return;
    }

    Client client1(clientPort);
    request += " " + sessionUserId;
    char *res = client1.sendRequest(tracker1.ip, tracker1.port, request,18888,0);

    string response(res);
    if (response == "success") {
      cout << "group created \n";
    } else {
      cout << response << endl;
    }
    close(client1.socket_status_id);
  }

  else if (!requestvec.empty() && requestvec[0] == "logout") {
    sessionUserId = "";
    cout << "logout Success" << endl;
  }

  else if (!requestvec.empty() && requestvec[0] == "list_groups") {

    Client client1(clientPort);

    char *res = client1.sendRequest(tracker1.ip, tracker1.port, request,18888,0);

    string response(res);

    cout << response << endl;
  }

  else if (!requestvec.empty() && requestvec[0] == "list_requests") {

    Client client1(clientPort);
    request += " " + sessionUserId;

    char *res = client1.sendRequest(tracker1.ip, tracker1.port, request,18888,0);

    string response(res);

    cout << response << endl;
  }

  else if (!requestvec.empty() && requestvec[0] == "join_group") {

    request += " " + sessionUserId;

    Client client1(clientPort);

    char *res = client1.sendRequest(tracker1.ip, tracker1.port, request,18888,0);

    string response(res);
    if (response == "success") {
      cout << "group joined  \n";
    } else {
      cout << response << endl;
    }
    close(client1.socket_status_id);
  }

  else if (!requestvec.empty() && requestvec[0] == "accept_request") {

    request += " " + sessionUserId;

    Client client1(clientPort);

    char *res = client1.sendRequest(tracker1.ip, tracker1.port, request,18888,0);

    string response(res);
    if (response == "success") {
      cout << "request accepted  \n";
    } else {
      cout << response << endl;
    }
    close(client1.socket_status_id);
  }

  else if (!requestvec.empty() && requestvec[0] == "leave_group") {

    request += " " + sessionUserId;

    Client client1(clientPort);

    char *res = client1.sendRequest(tracker1.ip, tracker1.port, request,18888,0);

    string response(res);
    if (response == "success") {
      cout << "left group  \n";
    } else {
      cout << response << endl;
    }
    close(client1.socket_status_id);
  }

  else if (!requestvec.empty() && requestvec[0] == "upload_file") {
    int uploadcomplete = 0;

    string filepath = requestvec[1];
    string groupid = requestvec[2];
    string filename = getFilenameFromPath(filepath);
    int numberOfPieces;
    string shaofall = calculateSHA1ForFile(filepath, numberOfPieces);
    cout << " no of pieces: " << numberOfPieces << endl;
    request += " " + sessionUserId;

    Client client1(clientPort);

    char *res = client1.sendRequest(tracker1.ip, tracker1.port, request,18888,0);

    string response(res);
    if (response == "success") {
      cout << "file data upadted in tracker  \n";

      for (int i = 0; i < numberOfPieces; i++) {
        string piece = shaofall.substr(i * 40, 40);
        cout << "sending piece: " << piece << endl;
        string tempreq = "uploadsha " + groupid + " " + filepath + " " + piece;
        Client client2(clientPort);
        char *tempres =
            client2.sendRequest(tracker1.ip, tracker1.port, tempreq,18888,0);
        string response(tempres);
        if (i == numberOfPieces - 1 && response == "success") {
          uploadcomplete = 1;
        }
        if (response == "success") {
          cout << "sha piece sent  \n";
        } else {
          cout << response << endl;
        }
        close(client1.socket_status_id);
      }

    } else {
      cout << response << endl;
    }

    if (uploadcomplete == 1) {
      long filesize = get_file_size_in_bytes(filepath.c_str());
      long leftoverBytes = filesize - ((filesize / pieceblock) * pieceblock);

      vector<bool> bitMap(numberOfPieces, true);
      filetobitmap[filepath] = make_pair(bitMap, leftoverBytes);
    }

    close(client1.socket_status_id);
  }

  else if (!requestvec.empty() && requestvec[0] == "list_files") {

    Client client1(clientPort);

    char *res = client1.sendRequest(tracker1.ip, tracker1.port, request,18888,0);

    string response(res);
    if (response == "success") {
      cout << "files recieved  \n";
    } else {
      cout << response << endl;
    }
    close(client1.socket_status_id);
  }

  else if (!requestvec.empty() && requestvec[0] == "download_file") {

    /*(user fetch)Fetch all users with atleast 1 piece
     ******************************************************************************/
    /******************************************************************************/

    string incominggroupname = requestvec[1];
    string incomingfilepath = requestvec[2];
    string inscomingdestfilepath = requestvec[3];
    string filename = getFilenameFromPath(incomingfilepath);

    request += " " + sessionUserId;

    Client client1(clientPort);

    char *res = client1.sendRequest(tracker1.ip, tracker1.port, request,18888,0);

    string response(res);
    if (response != "failure") {
      cout << "userswithfile info  recieved  \n";

      vector<string> alluserswithpieces = splitwordsbyspace(response);
      unordered_map<string, pair<string, int>>
          usertoipport; // all users info which have the current file, to be
                        // stored here

      /*(user - bitmap )Fetch bitmaps from all those users and store each bitmap
       *with each user
       ******************************************************************************/
      /******************************************************************************/

      for (auto eachuser : alluserswithpieces) {
        vector<string> tokens = splitwordsbydash(eachuser);
        int clientport = stoi(tokens[2].c_str());
        usertoipport[tokens[0]] = make_pair(tokens[1], clientport);

        cout << "userinfos::: " << tokens[0] << " " << tokens[1] << " "
             << tokens[2] << endl;
      }

      //  bitmap from all valid users, to be
      usertocurrentfileBitmapV2.clear(); //  stored here

      vector<thread> Threadbitmapfromeachuser;

      sem_t mutex;
      sem_init(&mutex, 0, 1);
      for (auto us : usertoipport) {
        string clientid = us.first;
        string clientip = us.second.first;
        int clientport = us.second.second;

        Threadbitmapfromeachuser.push_back(
            thread(bitmapfromeachuser, incominggroupname, clientid, clientip,
                   clientport, incomingfilepath, ref(usertocurrentfileBitmapV2),
                   ref(mutex)));
      }

      for (thread &t : Threadbitmapfromeachuser) {
        t.join();
      }
      sem_destroy(&mutex);
      Threadbitmapfromeachuser.clear();

      cout << "user to  each file bitmap  (usertocurrentfileBitmapV2) \n "
              "========================================================\n";
      for (auto i : usertocurrentfileBitmapV2) {
        cout << i.first << " " << i.second.first << " " << i.second.second
             << endl;
      }
      cout << endl;

      long noofpieces = usertocurrentfileBitmapV2.begin()->second.first.size();
      cout << "noofpieces ::: " << noofpieces << endl;

      /*(piece - users)for each piece , arrange it along with the users which
      have it (descending - piece selection algorithm)
      ******************************************************************************/
      /******************************************************************************/
      pieceToNoofpieceNseederList.assign(
          noofpieces, make_pair(0, make_pair(0, vector<string>{})));
      // pieceno - (noofoccurance - seeder1, seeder2,seeder3)
      for (auto userBitmap : usertocurrentfileBitmapV2) {

        for (int i = 0; i < noofpieces; i++) {
          if (userBitmap.second.first[i] == '1') {
            cout << "inside \n";
            pieceToNoofpieceNseederList[i].first = i;
            pieceToNoofpieceNseederList[i].second.first++;
            pieceToNoofpieceNseederList[i].second.second.push_back(
                userBitmap.first);
          }
        }
      }

      sort(pieceToNoofpieceNseederList.begin(),
           pieceToNoofpieceNseederList.end(), customComparator);

      cout << "piece -  priority - which user  (pieceToNoofpieceNseederList)  "
              "\n "
              "=========================================================\n";
      for (auto i : pieceToNoofpieceNseederList) {
        cout << i.first << " " << i.second.first << " ";
        for (auto j : i.second.second) {
          cout << j << " ";
        }
        cout << endl;
      }
      cout << endl;

      /*for each piece download it
      ******************************************************************************/
      /******************************************************************************/
      cout << "for each piece download\n";
      inscomingdestfilepath += "/";
      inscomingdestfilepath += filename;
      int writefiledesc =
          open(inscomingdestfilepath.c_str(), O_CREAT | O_RDWR, 0666);
      seedersofcurrentfile.clear();

      long leftoverbytes = usertocurrentfileBitmapV2.begin()->second.second;
      vector<bool> bitMap(noofpieces, false);
      filetobitmap[incomingfilepath] = make_pair(bitMap, leftoverbytes);

      vector<thread> downloadthreads;
      for (int i = 0; i < noofpieces; i++) {
        seedersofcurrentfile = pieceToNoofpieceNseederList[i].second.second;
        for (auto seeder : seedersofcurrentfile) {
          string ipofseeder = usertoipport[seeder].first;
          int portofseeder = usertoipport[seeder].second;
          int piecesize = pieceblock;

          if (i == noofpieces - 1)
            piecesize = leftoverbytes;
          cout << "sending request for: " << i << " th piece \n";
          downloadthreads.emplace_back(piecedownload, incomingfilepath,
                                       ipofseeder, portofseeder, i, piecesize,
                                       writefiledesc);
          filetobitmap[incomingfilepath].first[i] = true;
        }
      }
      for (thread &t : downloadthreads) {
        t.join();
      }

      string originalsha;
      if (1) {
        string  sharequest = "getshaoffile "+ incominggroupname+ " "+ incomingfilepath;

        Client tempclient1(clientPort);
        char* res=tempclient1.sendRequest(tracker1.ip, tracker1.port, sharequest,18888,0);
        string response(res);
        if (response == "failure") {
          cout << "sha not recieved  \n";
        } else {
          
          originalsha = response;
        }
        close(tempclient1.socket_status_id);
      }
      int temp5;
      string currsha = calculateSHA1ForFile(inscomingdestfilepath,temp5);

      cout<<"originalsha is:   ";
      cout<<originalsha<<endl;
      cout<<"currsha is:   ";
      cout<<currsha<<endl;

      if(currsha != originalsha){
        cout<<"fileintegrity violated";
        return ;
      }

      /*set this user as seeder in tracker
      ******************************************************************************/
      /******************************************************************************/
      cout << "set this user as seeder";
      if (1) {
        string request = "newseeder " + incominggroupname + " " +
                         incomingfilepath + " " + sessionUserId;
        Client tempclient1(clientPort);
        tempclient1.sendRequest(tracker1.ip, tracker1.port, request,18888,0);
        if (response == "success") {
          cout << "files recieved  \n";
        } else {
          cout << response << endl;
        }
        close(tempclient1.socket_status_id);
      }

    } else {
      cout << response << endl;
    }
    close(client1.socket_status_id);
  }
}

void clientAsServer(int port) {
  Server server1(port);
  struct sockaddr *address = (struct sockaddr *)&server1.address;
  socklen_t addressLen = (socklen_t)sizeof(server1.address);
  while (true) {
    int socketStatus = accept(server1.socket_status_id, address, &addressLen);

    char req[888];
    memset(req, 0, sizeof(req));
    read(socketStatus, req, sizeof(req));
    string incomingReq(req);

    vector<string> incomingReqvec = splitwordsbyspace(incomingReq);

    cout << "splitted recieved sentence:  ";
    for (auto i : incomingReqvec) {
      cout << i << " ";
    }
    cout << endl;

    if (!incomingReqvec.empty() && incomingReqvec[0] == "getbitmapfromClient") {
      cout << "getting bitmap\n";
      string incomingfilepath = incomingReqvec[1];
      if (filetobitmap.find(incomingfilepath) != filetobitmap.end()) {
        string response =
            convertToFormattedString(filetobitmap[incomingfilepath]);
        send(socketStatus, response.c_str(), 21, 0);
      } else {
        send(socketStatus, "failed, file bitmap does not exist", 21, 0);
        cout << "file bitmap does not exist";
      }
    } else if (!incomingReqvec.empty() && incomingReqvec[0] == "download") {
      cout << "recieved download request: \n";

     

      string incomingfilepath = incomingReqvec[1];
      int piecenumber = stoi(incomingReqvec[2]);

      long noofpieces = filetobitmap.begin()->second.first.size();
      int piecesize = pieceblock;
      if (piecenumber == noofpieces - 1) {
        piecesize = filetobitmap[incomingfilepath].second;
      }

      int file = open(incomingfilepath.c_str(), O_RDONLY);
      long offset = piecenumber * pieceblock;
      lseek(file, offset, SEEK_SET);

      char buffer[piecesize];
      read(file, buffer, piecesize);
      

      close(file);

      int sendsize =send(socketStatus, buffer, piecesize, 0);
      cout<<"piecesize sent: "<<sendsize<<endl;
      cout << " piece: " << piecenumber << " sent \n";
    }
  }
}

void bitmapfromeachuser(
    string incominggroupname, string clientid, string clientip, int clientport,
    string incomingfilepath,

    unordered_map<string, pair<string, int>> &usertocurrentfileBitmapV2,
    sem_t &mutex) {
  cout << "inside bitmapfromeachuser \n";

  Client client2(clientport);

  string request = "getbitmapfromClient " + incomingfilepath;

  char *res = client2.sendRequest(clientip, clientport, request,18888,0);
  string response(res);

  if (response != "failure") {

    vector<string> temp = splitwordsbyspace(response);

    sem_wait(&mutex);
    usertocurrentfileBitmapV2[clientid] =
        make_pair(temp[0], atoi(temp[1].c_str()));

    sem_post(&mutex);

  } else {
    cout << response << endl;
  }
  close(client2.socket_status_id);
}

void piecedownload(string filename, string ipofseeder, int portofseeder, int i,
                   int piecesize, int writefiledesc) {

  string request = "download " + filename + " " + to_string(i);
  Client clienttemp(portofseeder);
  char *response = clienttemp.sendRequest(ipofseeder, portofseeder, request,piecesize,1);
  string strres(response);
  cout<< "piecesize written: "<<strres.size()<<endl;
  pwrite(writefiledesc, response, piecesize, i * pieceblock);
  close(clienttemp.socket_status_id);
}
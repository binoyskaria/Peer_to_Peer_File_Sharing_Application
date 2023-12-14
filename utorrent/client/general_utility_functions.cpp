



#include "general_utility_functions.h"

using namespace std;

vector<string> splitwordsbyspace(string inputstring) {
  inputstring += " ";
  vector<string> inputstringchunks;
  string currentWord;
  for (char c : inputstring) {
    if (c == ' ') {
      if (!currentWord.empty()) {
        inputstringchunks.push_back(currentWord);
        currentWord.clear();
      }
    } else {
      currentWord += c;
    }
  }
  return inputstringchunks;
}


vector<string> splitwordsbydash(string inputstring) {
  inputstring += "-";
  vector<string> inputstringchunks;
  string currentWord;
  for (char c : inputstring) {
    if (c == '-') {
      if (!currentWord.empty()) {
        inputstringchunks.push_back(currentWord);
        currentWord.clear();
      }
    } else {
      currentWord += c;
    }
  }
  return inputstringchunks;
}

struct sockaddr_in getDefaultServerStructure() {
    struct sockaddr_in servaddr;

    FILE* file = fopen("tracker_info.txt", "r");
    if (file == nullptr) {
        perror("Error opening file");
        exit(1);
    }

    char ipBuffer[128];
    if (fgets(ipBuffer, sizeof(ipBuffer), file) == nullptr) {
        perror("Error reading IP");
        exit(1);
    }

    char portBuffer[10];
    if (fgets(portBuffer, sizeof(portBuffer), file) == nullptr) {
        perror("Error reading port");
        exit(1);
    }

    fclose(file);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ipBuffer);
    servaddr.sin_port = htons(atoi(portBuffer));

    return servaddr;
}

struct sockaddr_in getDefaultServerStructure1() {
  Tracker tracker1(1, "127.0.0.1", 2020);

  struct sockaddr_in servaddr;

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(tracker1.ip.c_str());
  servaddr.sin_port = htons(tracker1.port);

  return servaddr;
}

Tracker getTracker1() {
  Tracker tracker1(1, "127.0.0.1", 2020);

  return tracker1;
}

Tracker getTracker() {
    Tracker tracker1(1,"",0);
    FILE* file = fopen("tracker_info.txt", "r");
    if (file != nullptr) {
        char ipBuffer[128];
        if (fgets(ipBuffer, sizeof(ipBuffer), file) != nullptr) {
            tracker1.ip = std::string(ipBuffer);
        }

        char portBuffer[10];
        if (fgets(portBuffer, sizeof(portBuffer), file) != nullptr) {
            tracker1.port = atoi(portBuffer);
        }

        fclose(file);
    }
    return tracker1;
}



string calculateSHA1(const unsigned char* data, size_t length) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(data, length, hash);

    string hashStr;
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        char hex[3];
        snprintf(hex, sizeof(hex), "%02x", hash[i]);
        hashStr += hex;
    }
    return hashStr;
}


string calculateSHA1ForFile(const string& filepath, int& numberOfPieces) {
    FILE* file = fopen(filepath.c_str(), "rb");
    if (!file) {
        return "File not found or could not be opened.";
    }

    string hashString;
    const size_t chunkSize = 524288; // 524288 KB buffer
    vector<unsigned char> buffer(chunkSize);

    size_t bytesRead;
    numberOfPieces = 0; 

    while ((bytesRead = fread(buffer.data(), 1, chunkSize, file)) > 0) {
        hashString += calculateSHA1(buffer.data(), bytesRead);
        numberOfPieces++; 
    }

    fclose(file);
    return hashString;
}


string getFilenameFromPath(const string& filepath) {
    size_t found = filepath.find_last_of("/\\");
    if (found != string::npos) {
        return filepath.substr(found + 1);
    }
    return filepath; 
}

long get_file_size_in_bytes(const char* filepath) {
    FILE* file = fopen(filepath, "rb");

    if (!file) {
       
        return -1;
    }

    fseek(file, 0, SEEK_END); 
    long size = ftell(file); 

    fclose(file); // Close the file

    return size;
}

string convertToFormattedString(const pair<vector<bool>, int>& input) {
    const vector<bool>& boolVector = input.first;
    int intValue = input.second;

    string result;

    
    for (bool value : boolVector) {
        result += (value ? '1' : '0');
        
    }

   
    result +=" "+ to_string(intValue);

    return result;
}

bool customComparator(const pair<int, pair<int, vector<string>>>& a,
                     const pair<int, pair<int, vector<string>>>& b) {
    return a.second.first >= b.second.first;
}

void splitString( std::string& input, std::string& firstPart, std::string& secondPart) {
    size_t colonPos = input.find(':');
    if (colonPos != std::string::npos) {
        firstPart = input.substr(0, colonPos);
        secondPart = input.substr(colonPos + 1);
    } else {
        input="error";
    }
}
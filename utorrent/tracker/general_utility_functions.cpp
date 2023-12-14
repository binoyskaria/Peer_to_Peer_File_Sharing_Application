#include "general_utility_functions.h"

using namespace std;




vector<string> splitwordsbyspace(string inputstring) {
  inputstring+=" ";
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
  cout<<"words splitted successfully \n";
  return inputstringchunks;
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
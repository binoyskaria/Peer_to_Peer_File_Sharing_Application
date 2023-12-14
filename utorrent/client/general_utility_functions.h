#ifndef GENERAL_UTILITY_FUNCTIONS
#define GENERAL_UTILITY_FUNCTIONS


#include "blueprints.h"

using namespace std;

vector<string> splitwordsbyspace(string inputstring);

struct sockaddr_in getDefaultServerStructure();
Tracker getTracker();

string calculateSHA1(const unsigned char* data, size_t length);
string calculateSHA1ForFile(const string& filepath, int& numberOfPieces);
string getFilenameFromPath(const string& filepath);
long get_file_size_in_bytes(const char* filepath);
string convertToFormattedString(const pair<vector<bool>, int>& input);
vector<string> splitwordsbydash(string inputstring);
bool customComparator(const pair<int, pair<int, vector<string>>>& a,
                     const pair<int, pair<int, vector<string>>>& b);
void splitString( std::string& input, std::string& firstPart, std::string& secondPart) ;

#endif

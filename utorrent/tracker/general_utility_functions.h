#ifndef GENERAL_UTILITY_FUNCTIONS
#define GENERAL_UTILITY_FUNCTIONS

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
#include "blueprints.h"

std::vector<std::string> splitwordsbyspace(std::string inputstring);
Tracker getTracker();




#endif

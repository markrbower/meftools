#ifndef MEF_CONT
#define MEF_CONT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>
#include <list>

using namespace std;

class MEFcont {
private:
  string filename;
  string password;
  int bufferSize;
  vector<vector<int>> conts;
  int counter;
  int counterLimit;
  MEFinfo info;

public:
  MEFcont( string filename, string password, MEFinfo info, int bufferSize );
  bool hasNext();
  MEFiter next();

};

#endif

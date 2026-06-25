#ifndef MEF_CONT_H
#define MEF_CONT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>
#include <list>

#include "MEFinfo.h"
#include "MEFiter.h"

using namespace std;

struct MEFconts {
  long long startTime;
  long long timeStep;
  int startBlock;
  int stopBlock;
};

class MEFcont {
private:
  string filename;
  string password;
  int bufferSize;
  vector<MEFconts> conts;
  int counter;
  int counterLimit;
  MEFinfo info;
  vector<MEFconts> findContinuousSequences( vector<long long> timeConstraints );

public:
  MEFcont() {};
  MEFcont( MEFinfo info_, int bufferSize );
  bool hasNext();
  MEFiter next();
};

#endif

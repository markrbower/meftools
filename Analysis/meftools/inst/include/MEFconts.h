#ifndef MEF_CONTS_H
#define MEF_CONTS_H

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

struct MEFcont {
  long long startTime;
  long long timeStep;
  int startSample;
  int stopSample;
};

class MEFconts {
private:
  string filename;
  string password;
  int bufferSize;
  vector<MEFcont> conts;
  int counter;
  int counterLimit;
  MEFinfo info;
  vector<MEFcont> findContinuousSequences( vector<long long> timeConstraints );

public:
  MEFconts() {};
  MEFconts( MEFinfo info_, int bufferSize );
  bool hasNext();
  MEFiter next();
};

#endif

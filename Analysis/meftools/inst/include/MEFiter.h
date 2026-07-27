#ifndef MEF_ITER_H
#define MEF_ITER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>
#include <list>

#include "MEFinfo.h"

using namespace std;

class MEFiter {
private:
  long long time0, time1;
  int block0,block1,stepSize,Nrows,nBlocksPerStep,microsecondsPerSample;
  vector<int> starts, stops;
  int counter, counterLimit;
  MEFinfo info;

public:
  MEFiter( MEFinfo info_, int block0, int block1, int stepSize );
  bool hasNext();
  vector<int> next();
  long long timeStart() { return time0; };
  long long timeStop() { return time1; };
  long long timeStep() {
    double tmp = 1E6 / info.getHeader().sampling_frequency;
    long long value = static_cast<long long>( tmp );
    cout << "MEFiter: tmp: " << tmp << "\tvalue: " << value << endl;
    return value;
  } 

};
#endif


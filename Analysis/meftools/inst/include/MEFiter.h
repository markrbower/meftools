#ifndef MEF_ITER
#define MEF_ITER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>
#include <list>

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
  long long getTime() { return time0; };


};
#endif

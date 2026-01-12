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
  int block0,block1,stepSize,sampleSize,microsecondsPerSample;
  vector<int> starts, stops;
  int counter, counterLimit;
  MEFinfo info;

public:
  MEFiter( string filename, string password, MEFinfo info_, int block0, int block1, int stepSize );
  bool hasNext();
  vector<int> next();


};

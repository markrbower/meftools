#ifndef MEF_INFO_H
#define MEF_INFO_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>
#include <list>

#include "meftools_types.h"

using namespace std;

struct MEFconts {
  long long startTime;
  long long timeStep;
  int startBlock;
  int stopBlock;
};

class MEFinfo {
private:
  string filename, password;
  MEF_HEADER_INFO header;
  long long **ToC;
  vector<int> discontinuities;
  vector<string> variables;
public:
  MEFinfo( string filename, string password );
  vector<int> getDiscontinuities();
  vector<MEFconts> findContinuousMefSequences( vector<long long> timeConstraints );
  MEF_HEADER_INFO getHeader();
  long long** getToC();
  string getFilename();
  string getPassword();
};

#endif

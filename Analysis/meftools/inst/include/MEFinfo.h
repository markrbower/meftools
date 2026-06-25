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

class MEFinfo {
private:
  string filename, password;
  MEF_HEADER_INFO header;
  long long **ToC;
  vector<int> discontinuities;
  vector<string> variables;
public:
  MEFinfo() {};
  MEFinfo( string filename, string password );
  vector<int> getDiscontinuities();
  MEF_HEADER_INFO getHeader();
  long long** getToC();
  string getFilename();
  string getPassword();
  long long getStepSize();
};

#endif

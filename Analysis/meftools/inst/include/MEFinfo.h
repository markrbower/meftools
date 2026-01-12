#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>
#include <list>

using namespace std;

class MEFinfo {
private:
  string filename, password;
  MEF_HEADER_INFO header;
  long long **ToC;
  vector<int> discontinuities;
  vector<string> variables;
public:
  MEFinfo( string filename, string password );
  MEF_HEADER_INFO getHeader();
  long long** getToC();
  vector<int> getDiscontinuities();
  vector<vector<int>> findContinuousMefSequences( vector<long long> timeConstraints );
};

// MEFcont.cpp
// Mark R. Bower
// Yale University
/*
  #' Creates an iterator of contiguous MEF sequences.
  #' 
  #'  timeConstraints <- NPI:::checkTimeConstraints( compArgs_caseSpecific$get('info'), case )
  #'  iterCont <- meftools::MEFcont( filename, 'erlichda', compArgs_caseSpecific$get('bufferSize'), window=timeConstraints, info=compArgs_caseSpecific$get('info') )
*/
#include <string>
#include <iostream>
#include <limits>

#include "meftools_types.h"
#include "MEFinfo.h"
#include "MEFiter.h"

using namespace std;

class MEFcont {
private:
  string filename;
  string password;
  int bufferSize;
  MEFinfo info;
  vector<vector<int>> conts;
  int counter, counterLimit;

public:
  MEFcont( string filename, string password, MEFinfo info_, int bufferSize ) : info(info_) {
//    if ( info == NULL ) {
//      info = MEFinfo( filename, password );
//    }
    // Divide the continuous regions. Starts and Stops are inclusive.
    this->filename = filename;
    this->password = password;
    this->info = info_;
    this->bufferSize = bufferSize;

// No timeConstraints were given, so read the entire file
    vector<long long> timeConstraints = { 0, LLONG_MAX };

    this->conts = info.findContinuousMefSequences( timeConstraints );

    counter = 0;
    counterLimit = this->conts.size();
  }

  bool hasNext() {
      return( counter < counterLimit );
  };

  MEFiter next() {
      vector<int> vec = conts[counter];
      int block0 = vec[0];
      int block1 = vec[1];
      MEFiter it = MEFiter( filename, password, info, block0, block1, bufferSize );
      counter++;
      return( it ); 
  };

};


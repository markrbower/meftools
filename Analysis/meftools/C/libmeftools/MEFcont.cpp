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
#include <string>
#include <iostream>

#include "meftools_types.h"
#include "MEFinfo.h"
#include "MEFiter.h"

using namespace std;

class MEFcont {
private:
  int time0, time1, window, bufferSize;
  MEFinfo info;
  list<vector<int>> conts;
  MEFiter it;

public:
  MEFcont( char* filename, char* password, int bufferSize, vector<int> timeConstraints, MEFinfo info ) {
//    if ( info == NULL ) {
//      info = MEFinfo( filename, password );
//    }
    // Divide the continuous regions. Starts and Stops are inclusive.
    conts = info.findContinuousMefSequences( time0, time1 );
    it <- itertools::ihasNext( iterators::iter( conts, by="row" ) );
    


  }

};


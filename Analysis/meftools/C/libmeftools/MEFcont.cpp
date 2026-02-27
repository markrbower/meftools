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
#include "MEFcont.h"

using namespace std;

MEFcont::MEFcont( string filename_, string password_, MEFinfo info_, int bufferSize_ ) : info(info_) {
//    if ( info == NULL ) {
//      info = MEFinfo( filename, password );
//    }
    // Divide the continuous regions. Starts and Stops are inclusive.
    filename = filename_;
    password = password_;
    info = info_;
    bufferSize = bufferSize_;

// No timeConstraints were given, so read the entire file
    vector<long long> timeConstraints = { 0, LLONG_MAX };

    this->conts = info.findContinuousMefSequences( timeConstraints );

    counter = 0;
    counterLimit = this->conts.size();
  };

bool MEFcont::hasNext() {
    return( counter < counterLimit );
};

MEFiter MEFcont::next() {
    vector<int> vec = conts[counter];
    int block0 = vec[0];
    int block1 = vec[1];
    MEFiter it = MEFiter( filename, password, info, block0, block1, bufferSize );
    counter++;
    return( it ); 
};

long long MEFcont::time( int index ) {



};


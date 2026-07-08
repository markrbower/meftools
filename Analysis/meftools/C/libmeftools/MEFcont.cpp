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

MEFcont::MEFcont( MEFinfo info_, int bufferSize_ ) : info(info_) {
    cout << "Creating MEFcont" << endl;

    bufferSize = bufferSize_;

// No timeConstraints were given, so read the entire file
    vector<long long> timeConstraints = { 0, LLONG_MAX };

    this->conts = findContinuousSequences( timeConstraints );

    counter = 0;
    counterLimit = this->conts.size();

    cout << "MEFcont created." << endl;
};

vector<MEFconts> MEFcont::findContinuousSequences( vector<long long> timeConstraints ) {
    long long time0 = timeConstraints[0];
    long long time1 = timeConstraints[1];
    long long** toc;
    vector<long long> result;
    int microsecPerSample = 1E6/info.getHeader().sampling_frequency;
    cout << "Sample: " << microsecPerSample << endl;

    vector<MEFconts> conts;
    vector<int> contiguousStarts;
    vector<int> contiguousStops;
    vector<long long> dsamp;
    vector<int> disc = info.getDiscontinuities();
    int N = disc.size();
    cout << "nbr discontinuities: " << N << endl;

    cout << "Number of blocks: " << N << endl;
    toc = info.getToC();

    for ( int i=0; i<(N-1); i++ ) { // the last block cannot contain a start
      cout << i << endl;
      cout << toc[2][(i+1)] << "\t" << toc[2][(i)] << endl;
      dsamp.push_back( toc[2][(i+1)] - toc[2][i] );
      if ( toc[0][i] <= time1 ) {
        if ( toc[0][i] >= time0 ) {
          if ( disc[i] == 1 ) {
            contiguousStarts.push_back( i );
	    contiguousStops.push_back( i-1 );
          } 
        }
      }
    }
    contiguousStops.push_back( N-1 );
    contiguousStops.erase( contiguousStops.begin() );
    cout << "Starts: " << contiguousStarts.size() << "\tStops: " << contiguousStops.size() << endl;
    
    N = contiguousStarts.size();
    for ( int i=0; i<N; i++ ) {
      MEFconts tmp;
      tmp.startTime  = info.getToC()[2][contiguousStarts[i]];
      tmp.timeStep   = round( 1.0 / info.getHeader().sampling_frequency );
      tmp.startBlock = info.getToC()[0][contiguousStarts[i]];
      tmp.stopBlock  = info.getToC()[0][contiguousStops[i]] + dsamp[contiguousStops[i]]*microsecPerSample;
      conts.push_back( tmp );
    }
    return( conts );
}

bool MEFcont::hasNext() {
    return( counter < counterLimit );
};

MEFiter MEFcont::next() {
    MEFconts mefconts = conts[counter];
    int startBlock = mefconts.startBlock;
    int stopBlock = mefconts.stopBlock;
    MEFiter it = MEFiter( info, startBlock, stopBlock, bufferSize );
    counter++;
    return( it ); 
};


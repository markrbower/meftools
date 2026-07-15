/* MEFconts.cpp
   Mark R. Bower
   Yale University

  #' Creates an iterator of contiguous MEF sequences.
  #' 
  #'  timeConstraints <- NPI:::checkTimeConstraints( compArgs_caseSpecific$get('info'), case )
  #'  iterCont <- meftools::MEFconts( filename, 'erlichda', compArgs_caseSpecific$get('bufferSize'), window=timeConstraints, info=compArgs_caseSpecific$get('info') )
*/
#include <string>
#include <iostream>
#include <limits>

#include "meftools_types.h"
#include "MEFinfo.h"
#include "MEFiter.h"
#include "MEFconts.h"

using namespace std;

MEFconts::MEFconts( MEFinfo info_, int bufferSize_ ) : info(info_) {
    cout << "Creating MEFconts" << endl;

    bufferSize = bufferSize_;

// No timeConstraints were given, so read the entire file
    vector<long long> timeConstraints = { 0, LLONG_MAX };

    this->conts = findContinuousSequences( timeConstraints );

    counter = 0;
    counterLimit = this->conts.size();

    cout << "MEFconts created." << endl;
};

vector<MEFcont> MEFconts::findContinuousSequences( vector<long long> timeConstraints ) {
    // The term "continuous sequences" means "continuous blocks".
    // This differs from sequential timestamps. MEF files are read in blocks.
    // Read blocks and then pare samples to the specified time range.
    // So, the desired input is Time Range, because it is needed at the end to refine.

    long long time0 = timeConstraints[0];
    long long time1 = timeConstraints[1];
    long long** toc;
    vector<long long> result;
    int microsecPerSample = 1E6/info.getHeader().sampling_frequency;
    cout << "Sample: " << microsecPerSample << endl;

    vector<MEFcont> conts;
    vector<int> contiguousStarts;
    vector<int> contiguousStops;
    vector<long long> dsamp;
    vector<int> disc = info.getDiscontinuities();
    int N = disc.size();
    cout << "nbr discontinuities: " << N << endl;

    toc = info.getToC();

    // Whether the first and last have discontinuity flags or not,
    // they are still "starts" and "stops".
    int firstStart = 0;
    int lastStop = N;
    cout << "Time Window:\t" << time0 << "\t" << time1 << endl;
    for ( int i=0; i<(N-1); i++ ) { // the last block cannot contain a start
      cout << i << endl;
      cout << toc[0][(i)] << "\t" << toc[0][(i+1)] << endl;
      cout << toc[1][(i)] << "\t" << toc[1][(i+1)] << endl;
      cout << toc[2][(i)] << "\t" << toc[2][(i+1)] << "\t" << disc[i] << endl;
      dsamp.push_back( toc[2][(i+1)] - toc[2][i] );
      if ( toc[0][i]>=time0 && toc[0][i]<=time1 ) {
        cout << "In the window." << endl;
        if ( disc[i]==1 || firstStart==0 ) {
          cout << "A start occurs at: " << toc[0][i] << endl;
          contiguousStarts.push_back( i );
	  contiguousStops.push_back( i-1 );
          firstStart = 1;
        }
        lastStop = i;
      }
    }
    contiguousStops.push_back( lastStop );
    contiguousStops.erase( contiguousStops.begin() );
    cout << "Starts: " << contiguousStarts.size() << "\tStops: " << contiguousStops.size() << endl;
    
    N = contiguousStarts.size();
    for ( int i=0; i<N; i++ ) {
      MEFcont tmp;
      tmp.startTime  = toc[0][contiguousStarts[i]];
      tmp.timeStep   = round( 1.0 / info.getHeader().sampling_frequency );


// These should be blocks, not samples.


      tmp.startSample = toc[2][contiguousStarts[i]];
      tmp.stopSample  = toc[2][contiguousStops[i]] + dsamp[contiguousStops[i]]*microsecPerSample;
      cout << "findContinuousSequences: StartSample: " << tmp.startSample << "\tStopSample: " << tmp.stopSample << endl;
      conts.push_back( tmp );
    }
    return( conts );
}

bool MEFconts::hasNext() {
    return( counter < counterLimit );
};

MEFiter MEFconts::next() {
    MEFcont mefcont = conts[counter];
    int startSample = mefcont.startSample;
    int stopSample = mefcont.stopSample;
    cout << "MEFconts: StartSample: " << startSample << "\tStopSample: " << stopSample << endl;
    MEFiter it = MEFiter( info, startSample, stopSample, bufferSize );
    counter++;
    return( it ); 
};


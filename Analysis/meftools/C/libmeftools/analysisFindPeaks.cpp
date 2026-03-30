/* analysisFindPeaks.cpp

Implements MEFanalysis interface to find peaks in MEF data.

Mark R. Bower
Yale University
*/
#include <string>
#include <iostream>

#include "meftools_types.h"
#include "MEFinfo.h"
#include "MEFiter.h"
#include "MEFcont.h"
#include "MEFanalysis.h"
#include "CircularBuffer.h"
#include "analysisFindPeaks.h"

vector<int> decomp_mef( string f, long long s0, long long s1, string p );

analysisFindPeaks::analysisFindPeaks( CaseSpecificVariables csv_, AlgorithmSpecificVariables asv_, MEFcont cont_, CircularBuffer cb_ ) : cont(cont_), circbuf(cb_) {
        cont = cont_;
	csv = csv_;
	asv = asv_;
        analysisFindPeaks::circbuf = CircularBuffer( csv.bufferSize );
	dbcm = DatabaseConnectionManager( csv.subject );
}

void analysisFindPeaks::compute() {
// Use MEFcont to supply contiguous sequences
	int bufferLimit = 1000;

        // Iterate through the given section, find peaks, blackout, then store.
	while ( cont.hasNext() ) {
	    // I only need a ring buffer of size "window".
            MEFiter iter = cont.next();
            while ( iter.hasNext() ) {
                vector<int> startStop = iter.next();
		int start = startStop[0];
		int stop  = startStop[1];
                std::vector<int> data = decomp_mef( csv.filename, start, stop, csv.password );
                for ( int i=0; i<data.size(); i++ ) {
                    circbuf.push_back( data[i] );
	            if ( isPeak() ) {
                        // Find the timestamp from the index.
                        peakBuffer.push_back( iter.getTime() );
                        valuesBuffer.push_back( analysisFindPeaks::circbuf.getBuffer() );
                        // Check whether buffers should be written to MySQL
			if ( peakBuffer.size() > bufferLimit ) {
				store( peakBuffer, valuesBuffer );
				peakBuffer.clear();
				valuesBuffer.clear();
			}
                    }
                }
	    }
        }
	store( peakBuffer, valuesBuffer );
}

void MEFanalysis::store( vector<long long> peakBuffer, vector<vector<int>> valuesBuffer ) {
	dbcm.batchInsertTimeVector( peakBuffer, valueBuffer );
}

void MEFanalysis::performance() {
        // Implementation of MyFunction

}

void MEFanalysis::graph() {
        // Implementation of MyFunction

}

void analysisFindPeaks::push_back( long long x ) {
    circbuf.push_back( x );
}

bool analysisFindPeaks::isPeak() {
    return circbuf.isPeak();
}

vector<int> analysisFindPeaks::getBuffer() {
    return circbuf.getBuffer();
}

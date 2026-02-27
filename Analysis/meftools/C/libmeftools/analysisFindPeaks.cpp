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

analysisFindPeaks::analysisFindPeaks( CaseSpecificVariables csv, AlgorithmSpecificVariables asv, MEFcont cont_, CircularBuffer cb_ ) : cont(cont_), circbuf(cb_) {
        cont = cont_;
        analysisFindPeaks::circbuf = CircularBuffer( csv.bufferSize );
}

void analysisFindPeaks::compute() {
        // Iterate through the given section, find peaks, blackout, then store.
	while ( iter.hasNext() ) {
	    // I only need a ring buffer of size "window".
	    push_back( iter.next() );
	    if ( isPeak() ) {
                // Find the timestamp from the index.
                peakBuffer.push_back( iter.time() );
                valuesBuffer.push_back( analysisFindPeaks::circbuf.getBuffer() );
                // Check whether buffers should be written to MySQL

	    }
        }
}


void MEFanalysis::store() {
        // Check buffer, then store.

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

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
#include "DatabaseAccessor.h"

vector<int> decomp_mef( string f, long long s0, long long s1, string p );

analysisFindPeaks::analysisFindPeaks( CaseSpecificVariables csv_, AlgorithmSpecificVariables asv_, MEFcont cont_, CircularBuffer cb_ ) : cont(cont_), circbuf(cb_) {
        cont = cont_;
	csv = csv_;
	asv = asv_;
        analysisFindPeaks::circbuf = CircularBuffer( csv.bufferSize );
	dba = DatabaseAccessor("NPI");
	dba.createTable("peaks","(subject varchar(32),session varchar(32),time bigint,waveform varchar(256))" );
}

void analysisFindPeaks::compute() {
// Use MEFcont to supply contiguous sequences
	char charArray[20];
	map<long long, string> peaks;
	int bufferLimit = 100;

        // Iterate through the given section, find peaks, blackout, then store.
	while ( cont.hasNext() ) {
	    // I only need a ring buffer of size "window".
            MEFiter iter = cont.next();
            while ( iter.hasNext() ) {
                vector<int> startStop = iter.next();
		int start = startStop[0];
		int stop  = startStop[1];
                std::vector<int> data = decomp_mef( csv.filename, start, stop, csv.password );

		// Filter


                for ( int i=0; i<data.size(); i++ ) {
                    circbuf.push_back( data[i] );
	            if ( isPeak() ) {
			vector<int> values = circbuf.getBuffer();
			string valueString;
			for ( auto v: values ) {
				sprintf( charArray, "%s,", value );
				valueString.append( charArray );
			}
			valueString.resize( valueString.size() - 1 );
			peaks.insert( { circbuf.getTime(), valueString } );
                        // Check whether buffers should be written to MySQL
			if ( peaks.size() > bufferLimit ) {
				dba.store( "peaks", buffer );
				buffer.clear();
				bufferCount = 0;
			}
                    }
                }
	    }
        }
	if ( peaks.size() > 0 )
		dba.store( "peaks", peaks );
}

void MEFanalysis::store( vector<long long> peakBuffer, vector<vector<int>> valuesBuffer ) {


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

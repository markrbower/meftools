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
#include "MEFconts.h"
#include "MEFanalysis.h"
#include "CircularBuffer.h"
#include "CircularBufferMEF.h"
#include "analysisFindPeaks.h"
#include "DatabaseAccessor.h"
#include "FiltFilt.h"

#include "CircularBufferMEF_allPeaks.h"

vector<int> decomp_mef( string f, long long s0, long long s1, string p );

using namespace std;

analysisFindPeaks::analysisFindPeaks( CaseSpecificVariables csv_, AlgorithmSpecificVariables asv_, MEFinfo info_, MEFconts conts_, CircularBufferMEF_allPeaks cb_ ) : circbuf(cb_) {
	csv = csv_;
	asv = asv_;
	conts = conts_;
	info = info_;

//        analysisFindPeaks::circbuf = CircularBufferMEF( csv.bufferSize, 0, 0 );
	const char* npi = "NPI";
	dba = DatabaseAccessor( npi );

    	dba.runSQL("create database if not exists NPI;");
    	dba.runSQL("use NPI;");

    	dba.runSQL("drop table if exists peaks;");

    	dba.runSQL("create table peaks (subject varchar(64), session varchar(64), time bigint, waveform varchar(256));" );

	cout << "Database table created." << endl;
}

void analysisFindPeaks::compute() {
// Use MEFconts to supply contiguous sequences
	char charArray[20];
	map<long long, string> peaks;
	map<string,string> fixed;
	fixed["subject"] = csv.subject;
	fixed["session"] = csv.session;
	int bufferLimit = 100;

	// Filter coefficients for what frequency band?
	kb::math::FilterCoefficients<double> fc_unit{ 
        	.m_CoefficientsA = {1.0000,-2.374094743709352,1.929355669091215,-0.532075368312092}, 
	        .m_CoefficientsB = {2.898194633721429e-03,8.694583901164288e-03,8.694583901164288e-03,2.898194633721429e-03}
	};
	kb::math::FilterCoefficients<double> fc_iis{ 
        	.m_CoefficientsA = {1.0000,-2.374094743709352,1.929355669091215,-0.532075368312092}, 
	        .m_CoefficientsB = {2.898194633721429e-03,8.694583901164288e-03,8.694583901164288e-03,2.898194633721429e-03}
	};
	// Select filter coefficients for the desired signal
	kb::math::FilterCoefficients<double> fc;
	if ( asv.signalType == "singleUnits" ) { // 0.6 - 6 kHz
		fc = fc_unit;
	} else if ( asv.signalType == "IIS" ) {  // 0.5 - 200 Hz
		fc = fc_iis;
	}
	kb::math::FiltFilt<double> filtfilt(fc);

        // Iterate through the given section, find peaks, blackout, then store.
        cout << "AFP: starting cont loop" << endl;
	while ( conts.hasNext() ) {
            cout << "AFP: in a cont. Getting MEFiter." << endl;
	    // I only need a ring buffer of size "window".
            MEFiter iter = conts.next();


            // What is in this "cont"?
            // Why isn't the MEFiter running anything?


            while ( iter.hasNext() ) {
                cout << "AFP: in an iter" << endl;
		vector<int> data = iter.next();
		std::vector<double> signal(data.begin(), data.end());

		// How do I get timeStart? From the "Table of Contents" and block numbers?
		// Could ToC be put into "csv"?
	    	analysisFindPeaks::circbuf.reset( iter.timeStart(), iter.timeStep() );

		// Filter: need to convert data[type int] to signal[type double].
		auto zeroPhaseFiltered = filtfilt.ZeroPhaseFiltering(signal);

                for ( int i=0; i<zeroPhaseFiltered.size(); i++ ) {
                    analysisFindPeaks::circbuf.push_back( zeroPhaseFiltered[i] );
	            if ( analysisFindPeaks::circbuf.isPeak() ) {
			vector<double> values = analysisFindPeaks::circbuf.getBuffer();
			string valueString;
			for ( auto v: values ) {
				snprintf( charArray, 10, "%f,", v );

				valueString.append( charArray );
			}
			valueString.resize( valueString.size() - 1 );
			peaks.insert( { analysisFindPeaks::circbuf.getTime(), valueString } );
                        // Check whether buffers should be written to MySQL
			if ( peaks.size() > bufferLimit ) {
				dba.mapInsert( "peaks", fixed, peaks );
				peaks.clear();
			}
                    }
                }
	    }
        }
	if ( peaks.size() > 0 )
		dba.mapInsert( "peaks", fixed, peaks );
}

void analysisFindPeaks::store( vector<long long> peakBuffer, vector<vector<int>> valuesBuffer ) {


}

void analysisFindPeaks::performance() {
        // Implementation of MyFunction

}

void analysisFindPeaks::graph() {
        // Implementation of MyFunction

}

void analysisFindPeaks::push_back( long long x ) {
    circbuf.push_back( x );
}

bool analysisFindPeaks::isPeak() {
    return analysisFindPeaks::circbuf.isPeak();
}

vector<double> analysisFindPeaks::getBuffer() {
    return analysisFindPeaks::circbuf.getBuffer();
}

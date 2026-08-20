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
#include "CircularBufferMEF_allPeaks.h"
#include "analysisFindPeaks.h"
#include "DatabaseAccessor.h"
#include <kfr/base.hpp>
#include <kfr/dft.hpp>
#include <kfr/dsp/iir_design.hpp>
#include <kfr/io/python_plot.hpp>

vector<int> decomp_mef( string f, long long s0, long long s1, string p );

using namespace std;

analysisFindPeaks::analysisFindPeaks( CaseSpecificVariables csv_, AlgorithmSpecificVariables asv_, MEFinfo info_, MEFconts conts_, CircularBufferMEF_allPeaks cb_ ) : circbuf(cb_) {
	csv = csv_;
	asv = asv_;
	conts = conts_;
	info = info_;

	cout << "Sampling Frequency: " << info.getHeader().sampling_frequency << endl;

	const char* npi = "NPI";
	dba = DatabaseAccessor( npi );

    	dba.runSQL("create database if not exists NPI;");
    	dba.runSQL("use NPI;");

    	dba.runSQL("drop table if exists peaks;");

    	dba.runSQL("create table peaks (subject varchar(64), session varchar(64), time bigint, peakValue double, waveform varchar(2048));" );

	cout << "Database table created." << endl;
}

void analysisFindPeaks::compute() {
// Use MEFconts to supply contiguous sequences
	char charArray[20];
	map<long long, map<string,string>> peaks;
	map<string,string> inner_map;
	map<string,string> fixed;
	fixed["subject"] = csv.subject;
	fixed["session"] = csv.session;
	int MySQLbufferLimit = 1000;
        long long stepSize = info.getStepSize();
        CircularBufferMEF_allPeaks rawbuf = CircularBufferMEF_allPeaks( 51, 0L, stepSize );

	// KFR filtering
	kfr::zpk filt_lo = kfr::iir_lowpass(kfr::butterworth(5), 6000, 30000 );
	kfr::zpk filt_hi = kfr::iir_highpass(kfr::butterworth(5), 600, 30000 );
	// Convert to second-order sections
	kfr::iir_params<float> params_lo = kfr::to_sos<float>(filt_lo);
	kfr::iir_params<float> params_hi = kfr::to_sos<float>(filt_hi);

        // Iterate through the given section, find peaks, blackout, then store.
        cout << "AFP: starting cont loop" << endl;
	while ( conts.hasNext() ) {
            cout << "AFP: in a cont. Getting MEFiter." << endl;
	    // I only need a ring buffer of size "window".
            MEFiter iter = conts.next();

            while ( iter.hasNext() ) {
                cout << "AFP: in an iter" << endl;
		vector<int> data = iter.next();
		cout << "Got the data from the iter object." << endl;
		cout << "Amount of data read: " << data.size() << endl;

		// KFR filtering
		kfr::univector<float> uv_signal = kfr::make_univector( data );
		kfr::filtfilt( uv_signal, params_lo );	
		kfr::filtfilt( uv_signal, params_hi );	

		// How do I get timeStart? From the "Table of Contents" and block numbers?
		// Could ToC be put into "csv"?
		cout << "circbuf try to reset time" << endl;
	    	analysisFindPeaks::circbuf.reset( iter.timeStart(), iter.timeStep() );
		rawbuf.reset( iter.timeStart(), iter.timeStep() );
		cout << "circbuf time reset" << endl;

		// Filter: need to convert data[type int] to signal[type double].
		//auto zeroPhaseFiltered_tmp = filtfilt1.ZeroPhaseFiltering(signal);
		//auto zeroPhaseFiltered = filtfilt2.ZeroPhaseFiltering(zeroPhaseFiltered_tmp);
		cout << "Signal filtered." << endl;

                for ( int i=0; i<uv_signal.size(); i++ ) {
                //for ( int i=0; i<zeroPhaseFiltered.size(); i++ ) {
                    //cout << "i: " << i << "\t" << zeroPhaseFiltered[i] << endl;
                    analysisFindPeaks::circbuf.push_back( uv_signal[i] );
		    rawbuf.push_back( data[i] );
                    //cout << "pushed back" << endl;
	            if ( analysisFindPeaks::circbuf.isPeak() ) {
                        cout << i << " is a peak" << endl;
			vector<double> values = analysisFindPeaks::circbuf.getBuffer();
			vector<double> raw_values = rawbuf.getBuffer();
                        cout << "buffer gotten" << endl;
			string valueString;
			for ( auto v: values ) {
				snprintf( charArray, 12, "%.3f,", v );
				valueString.append( charArray );
			}
			string rawValueString;
			for ( auto v: raw_values ) {
				snprintf( charArray, 12, "%.3f,", v );
				rawValueString.append( charArray );
			}
                        cout << "resize" << endl;
			valueString.resize( valueString.size() - 1 );
			rawValueString.resize( rawValueString.size() - 1 );
			cout << "valueString: " << valueString << endl;
			cout << "rawValueString: " << rawValueString << endl;
			inner_map[ "waveform" ] = valueString;
			inner_map[ "peakValue" ] = to_string( analysisFindPeaks::circbuf.getMid() );
			peaks.insert( { analysisFindPeaks::circbuf.getTime(), inner_map } );
                        // Check whether buffers should be written to MySQL
			if ( peaks.size() > MySQLbufferLimit ) {
				dba.mapInsert( "peaks", fixed, peaks );
				peaks.clear();
			}
                    }
                }
	    }
        }
	if ( peaks.size() > 0 ) {
		dba.mapInsert( "peaks", fixed, peaks );
		peaks.clear();
	}
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

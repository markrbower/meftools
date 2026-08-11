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

//        analysisFindPeaks::circbuf = CircularBufferMEF( csv.bufferSize, 0, 0 );
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
	int bufferLimit = 100;

	// Filter coefficients for what frequency band?
/*
	kb::math::FilterCoefficients<double> fc_unit{ 
        	.m_CoefficientsA = {1.0000,  -2.9164,   2.8363,  -0.9198},
	        .m_CoefficientsB = {0.0875e-04,   0.2625e-04,   0.2625e-04,   0.0875e-04}
	};
	kb::math::FilterCoefficients<double> fc_iis{ 
        	//.m_CoefficientsA = {1.0000,  -2.9164,   2.8363,  -0.9198},
	        //.m_CoefficientsB = {0.0875E-04,   0.2625E-04,   0.2625E-04,   0.0875E-04}
        	.m_CoefficientsA = {1.0000,  -2.9164,   2.8363,  -0.9198},
	        .m_CoefficientsB = {0.0875E-04,   0.2625E-04,   0.2625E-04,   0.0875E-04}
	};
	kb::math::FilterCoefficients<double> fc_iis_highpass_1Hz{ 
        	.m_CoefficientsA = {1.0000,  -2.9996,   2.9992,  -0.9996},
	        .m_CoefficientsB = {0.9998,  -2.9994,   2.9994,  -0.9998}
        	//.m_CoefficientsA = {1.0000,  -4.9993,   9.9973,  -9.9959,   4.9973,  -0.9993},
	        //.m_CoefficientsB = {0.9997,  -4.9983,   9.9966,  -9.9966,   4.9983,  -0.9997}
	};
	kb::math::FilterCoefficients<double> fc_iis_highpass_5Hz{ 
        	.m_CoefficientsA = {1.0000,  -2.9979,   2.9958,  -0.9979},
	        .m_CoefficientsB = {0.9990,  -2.9969,   2.9969,  -0.9990}
        	//.m_CoefficientsA = {1.0000,  -4.9966,   9.9865,  -9.9797,   4.9865,  -0.9966},
	        //.m_CoefficientsB = {0.9983,  -4.9915,   9.9831,  -9.9831,   4.9915,  -0.9983}
	};
	// Select filter coefficients for the desired signal
	kb::math::FilterCoefficients<double> fc;
	kb::math::FilterCoefficients<double> fc1;
	kb::math::FilterCoefficients<double> fc2;
	if ( asv.signalType == "singleUnits" ) { // 0.6 - 6 kHz
		fc = fc_unit;
	} else if ( asv.signalType == "IIS" ) {  // 0.5 - 200 Hz
		//fc = fc_iis;
		fc1 = fc_iis_highpass_1Hz;
		fc2 = fc_iis;
	}
	kb::math::FiltFilt<double> filtfilt(fc);
	kb::math::FiltFilt<double> filtfilt1(fc1);
	kb::math::FiltFilt<double> filtfilt2(fc2);
*/

	// KFR filtering
	kfr::zpk filt = kfr::iir_lowpass(kfr::butterworth(5), 200, 30000);
	// Convert to second-order sections
	kfr::iir_params<float> params = kfr::to_sos<float>(filt);

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
		//std::vector<double> signal(data.begin(), data.end());
		//cout << "Amount of data read: " << data.size() << endl;

		// KFR filtering
		kfr::univector<float> uv_signal = kfr::make_univector( data );
		kfr::filtfilt( uv_signal, params );	


		// How do I get timeStart? From the "Table of Contents" and block numbers?
		// Could ToC be put into "csv"?
		cout << "circbuf try to reset time" << endl;
	    	analysisFindPeaks::circbuf.reset( iter.timeStart(), iter.timeStep() );
		cout << "circbuf time reset" << endl;

		// Filter: need to convert data[type int] to signal[type double].
		//auto zeroPhaseFiltered_tmp = filtfilt1.ZeroPhaseFiltering(signal);
		//auto zeroPhaseFiltered = filtfilt2.ZeroPhaseFiltering(zeroPhaseFiltered_tmp);
		cout << "Signal filtered." << endl;

                for ( int i=0; i<uv_signal.size(); i++ ) {
                //for ( int i=0; i<zeroPhaseFiltered.size(); i++ ) {
                    //cout << "i: " << i << "\t" << zeroPhaseFiltered[i] << endl;
                    analysisFindPeaks::circbuf.push_back( uv_signal[i] );
                    //cout << "pushed back" << endl;
	            if ( analysisFindPeaks::circbuf.isPeak() ) {
                        cout << i << " is a peak" << endl;
			vector<double> values = analysisFindPeaks::circbuf.getBuffer();
                        cout << "buffer gotten" << endl;
			string valueString;
			for ( auto v: values ) {
				snprintf( charArray, 12, "%.3f,", v );
				valueString.append( charArray );
			}
                        cout << "resize" << endl;
			valueString.resize( valueString.size() - 1 );
			cout << "valueString: " << valueString << endl;
			inner_map[ "waveform" ] = valueString;
			inner_map[ "peakValue" ] = to_string( analysisFindPeaks::circbuf.getMid() );
			peaks.insert( { analysisFindPeaks::circbuf.getTime(), inner_map } );
                        // Check whether buffers should be written to MySQL
			if ( peaks.size() > bufferLimit ) {
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

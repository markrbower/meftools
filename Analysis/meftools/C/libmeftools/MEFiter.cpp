/* MEFiter.cpp
   Mark R. Bower
   Yale University

   This class is intended to be called by MEFcont.cpp,
   which breaks MEF files into a vector of continuous sequences.
   This class assumes that the data in [ block0, block1 ] inclusive
   consists of continuous data.

*/
#include <string>
#include <iostream>

#include "meftools_types.h"
#include "MEFinfo.h"
#include "MEFiter.h"

using namespace std;

/*
stepSize: max number of seconds per step
*/

vector<int> decomp_mef( string f, long long s0, long long s1, string p );

MEFiter::MEFiter( MEFinfo info_, int block0, int block1, int stepSize ) : info(info_) {
	this->time0 = 0;
	this->time1 = LLONG_MAX;
	this->block0 = block0;
	this->block1 = block1;
	this->info   = info_;
	this->stepSize = stepSize;
	this->Nrows = info.getHeader().number_of_index_entries;
	cout << "MEFiter constructor: Nrows: " << this->Nrows << endl;

	this->nBlocksPerStep = round( stepSize / (this->info.getHeader().block_interval * 1E6) );
	this->microsecondsPerSample = 1E6 / this->info.getHeader().sampling_frequency;

	// Compute a stepSize that will distribute points equally among blocks
	int nSteps;
	float fSteps = (block1 - block0) / this->nBlocksPerStep;
	if ( fSteps - floor(fSteps) > 0.5 ) { // round up
		nSteps = ceil( fSteps );
	} else {
		nSteps = floor( fSteps );
	}
	if ( nSteps == 0 ) { nSteps = 1; }
	cout << "MEFiter: nSteps: " << nSteps << endl;
		
	this->stepSize = ceil( this->nBlocksPerStep / nSteps );
	int nextStart; 
	starts.push_back( block0 );
	for ( int step=0; step<nSteps; step++ ) {
		nextStart = starts[step] + stepSize;
		stops.push_back( nextStart - 1 );
		starts.push_back( nextStart );
	}
	starts.pop_back();
	cout << "MEFiter: nbr or starts: " << starts.size() << endl;
	counter = 0;
	counterLimit = starts.size();
};

bool MEFiter::hasNext() { // Are there more data chunks in this continuous stretch?
	return( counter < counterLimit );
};

vector<int> MEFiter::next() {
	vector<int> data;

	cout << "MEFiter: stops[counter]: " << stops[counter] << "\tNrows: " << Nrows << endl;
	block0 = starts[counter];
	if ( stops[counter] > Nrows ) {
		block1 = Nrows-1;
	} else {
		block1 = stops[counter];
	}
	long long** ToC = info.getToC();
	long long tmp = ToC[ 2 ][ block0 ];
	int s0 = static_cast<int>( tmp );
	int s1;
	if ( block1 >= (Nrows-1) ) {
		s1 = info.getHeader().number_of_samples - 1;
	} else {
		s1 = (int) info.getToC()[ 2 ][ (block1+1) ] - 1;
	}	
	// dlast is the number of samples in the last block
	int dlast = s1 - (int) info.getToC()[ 2 ][ block1 ] + 1;

	cout << "decomp_mef: " << s0 << "\t" << s1 << endl;
    	data = decomp_mef( info.getFilename(), s0, s1, info.getPassword() );
	cout << "From decomp_mef, the amount of data read: " << data.size() << endl;

	// Trim the data to exact timestamp requests
	long long blockTimeStart = info.getToC()[0][block0];
	long long blockTimeStop = info.getToC()[0][block1] + (long long)round(dlast*1E6*microsecondsPerSample);
	cout << "TimeStartStop: " << blockTimeStart << "\t" << blockTimeStop << endl;
	cout << "window: " << time0 << "\t" << time1 << endl;
	if ( blockTimeStart<=time0 & time0<=blockTimeStop ) { // requested start is within decoded data
		int bad = ceil( (time0 - blockTimeStart) / microsecondsPerSample );
		data.erase( data.begin(), data.begin() + bad );
	}
	if ( blockTimeStart<=time1 & time1<=blockTimeStop ) { // requested stop is within decoded data
		int bad = ceil( (blockTimeStop - time1) / microsecondsPerSample );
		data.resize( data.size() - bad ); 
	}
	cout << "time trimming done." << endl;

	// Add metadata regarding what you have downloaded

	counter++;
	return( data );
};

long long time() {
    return 0L;
}


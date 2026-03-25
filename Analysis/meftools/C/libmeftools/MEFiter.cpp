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
	this->time1 = 1E20;
	this->block0 = block0;
	this->block1 = block1;
	this->info   = info_;
	this->stepSize = stepSize;
	this->Nrows = round( sizeof( this->info.getToC() ) / ( 3 * sizeof(int) ) );

	this->nBlocksPerStep = round( stepSize / (this->info.getHeader().block_interval * 1E6) );
	this->microsecondsPerSample = 1E6 / this->info.getHeader().sampling_frequency;

	// Compute a stepSize that will distribute points equally among blocks
	int nBlocks;
	float fBlocks = (block1 - block0) / this->nBlocksPerStep;
	if ( fBlocks - floor(fBlocks) > 0.5 ) { // round up
		nBlocks = ceil( fBlocks );
	} else {
		nBlocks = floor( fBlocks );
	}
		
	this->stepSize = ceil( this->nBlocksPerStep / nBlocks );
	int nextStart; 
	starts.push_back( block0 );
	for ( int block=0; block<nBlocks; block++ ) {
		nextStart = starts[block] + stepSize;
		stops.push_back( nextStart - 1 );
		starts.push_back( nextStart );
	}
	starts.pop_back();
	counter = 0;
	counterLimit = starts.size();
};

bool MEFiter::hasNext() { // Are there more data chunks in this continuous stretch?
	return( counter < counterLimit );
};

vector<int> MEFiter::next() {
	vector<int> data;

	block0 = starts[counter];
	if ( stops[counter] > Nrows ) {
		block1 = Nrows-1;
	} else {
		block1 = stops[counter];
	}
	long long** ToC = info.getToC();
	long long tmp = ToC[ 3 ][ block0 ];
	int s0 = static_cast<int>( tmp );
	int s1;
	if ( block1 == Nrows ) {
		s1 = info.getHeader().number_of_samples;
	} else {
		s1 = (int) info.getToC()[ 3 ][ (block1+1) ] - 1;
	}	
	// dlast is the number of samples in the last block
	int dlast = s1 - (int) info.getToC()[ 3 ][ block1 ] + 1;

    	data = decomp_mef( info.getFilename(), s0, s1, info.getPassword() );

	// Trim the data to exact timestamp requests
	vector<long long> blockTime(2);
	blockTime.push_back( info.getToC()[1][block0] );
	blockTime.push_back( info.getToC()[1][block1] + (long long)round(dlast*1E6/info.getHeader().sampling_frequency) );
	if ( blockTime[1]<=time0 & time0<=blockTime[2] ) { // requested start is within decoded data
		int bad = ceil( (time0 - blockTime[1]) / microsecondsPerSample );
		data.erase( data.begin(), data.begin() + bad );
	}
	if ( blockTime[1]<=time1 & time1<=blockTime[2] ) { // requested stop is within decoded data
		int bad = ceil( (blockTime[2] - time1) / microsecondsPerSample );
		data.resize( data.size() - bad ); 
	}

	// Add metadata regarding what you have downloaded

	counter++;
	return( data );
};

long long time() {
    return 0L;
}


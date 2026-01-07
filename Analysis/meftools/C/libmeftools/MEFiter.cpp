#include <string>
#include <iostream>

#include "meftools_types.h"
#include "MEFinfo.h"
#include "VectorIterator.h"

using namespace std;

class MEFiter {
private:
	int block0,block1,stepSize,sampleSize,microsecondsPerSample;
	long long time0,time1;
	MEFinfo info;
	vector<int> starts, stops;
	int counter, counterLimit;

public:
	MEFiter( char* filename, char* password, MEFinfo info, int block0, int block1, int time0, int time1, int stepSize ) {
		this->block0 = block0;
		this->block1 = block1;
		this->time0  = time0;
		this->time1  = time1;
		this->info   = info;
		this->stepSize = stepSize;

		this->sampleSize = round( info.getHeader().block_interval * stepSize / 1E6 );
		this->microsecondsPerSample = 1E6 / info.getHeader().sampling_frequency;

		// Compute a stepSize that will distribute points equally among blocks
		int nBlocks;
		float fBlocks = (block1 - block0) / this->sampleSize;
		if ( fBlocks - floor(fBlocks) > 0.5 ) { // round up
			nBlocks = ceil( fBlocks );
		} else {
			nBlocks = floor( fBlocks );
		}
		
		this->stepSize = ceil( this->sampleSize / nBlocks );
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

	bool hasNext() {
		return( counter < counterLimit );
	};


	vector<int> next() {
		vector<int> result;
		result.push_back( starts[counter] );
		result.push_back( stops[counter] );
		counter++;
		return( result );
	};

};


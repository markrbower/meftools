#ifndef ALGORITHM_SPECIFIC_FACTORY
#define ALGORITHM_SPECIFIC_FACTORY

/* AlgorithmSpecificFactory.h

Implements factory for constructing analysis objects.

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
#include "CircularBufferMEF_allPeaks.h"

#include "FiltFilt.h"

class AlgorithmSpecificFactory {
private:
	MEFinfo info;
	MEFcont mefConts;
	CircularBuffer circbuf;
	DatabaseAccessor dba;

public:
	void set( string key, string value );
	void set( string key, double value )
	void set( string key, int value );
	void set( string key, long long value );
	void setAnalysisName( string name );
	CircularBuffer getCircularBuffer();
	MEFcont getMEFcont();
	MEFinfo getMEFinfo();	
	DatabaseAccessor getDatabaseAccessor();
};

#endif

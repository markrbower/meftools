#ifndef ALGORITHM_SPECIFIC_FACTORY
#define ALGORITHM_SPECIFIC_FACTORY

/* AlgorithmSpecificFactory.h

Implements factory for constructing analysis objects.

Mark R. Bower
Yale University
*/
#include <string>
#include <iostream>
#include <map>

#include "meftools_types.h"
#include "MEFinfo.h"
#include "MEFiter.h"
#include "MEFconts.h"
#include "MEFinfo.h"
#include "MEFanalysis.h"

#include "CircularBuffer.h"
#include "CircularBufferMEF_allPeaks.h"
#include "DatabaseAccessor.h"

class AlgorithmSpecificFactory {
private:
	map<string,string> S;
	map<string,int> I;
	map<string,double> D;
	map<string,long long> LL;

	MEFinfo* info = NULL;
	MEFconts* mefConts = NULL;
	CircularBuffer* circbuf = NULL;
	DatabaseAccessor* dba = NULL;

	string analysisName;

public:
	AlgorithmSpecificFactory();
	void set( string key, string value );
	void set( string key, double value );
	void set( string key, int value );
	void set( string key, long long value );
	void setAnalysisName( string name );
	CircularBuffer getCircularBuffer();
	MEFconts getMEFconts();
	MEFinfo getMEFinfo();	
	DatabaseAccessor getDatabaseAccessor();
};

#endif

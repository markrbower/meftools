#include <iostream>
#include <vector>
#include <mysql/mysql.h>
#include <mysqlx/xdevapi.h>
#include "MEFconts.h"
#include "DatabaseAccessor.h"

#include "AlgorithmSpecificFactory.h"

using namespace std;

AlgorithmSpecificFactory::AlgorithmSpecificFactory() {
	
}

// For all "set" methods, only set the value if the key is not already in the map.
void AlgorithmSpecificFactory::set( string key, string value ) {
	if (S.find(key) == S.end()) {
		S[ key ] = value;
	}
}

void AlgorithmSpecificFactory::set( string key, double value ) {
	if (D.find(key) == D.end()) {
		D[ key ] = value;
	}
}

void AlgorithmSpecificFactory::set( string key, int value ) {
	if (I.find(key) == I.end()) {
		I[ key ] = value;
	}
}

void AlgorithmSpecificFactory::set( string key, long long value ) {
	if (LL.find(key) == LL.end()) {
		LL[ key ] = value;
	}
}

void AlgorithmSpecificFactory::setAnalysisName( string n ) {
	analysisName = "findPeaks";
}

CircularBuffer AlgorithmSpecificFactory::getCircularBuffer() {
	if ( &circbuf == NULL ) {
		if ( analysisName == "findPeaks" ) {
			*circbuf = (CircularBuffer)CircularBufferMEF_allPeaks( I["bufferCapacity"], LL["startTime"], LL["stepTime"] );
			return *circbuf;
		}
	} else {
		return *circbuf;
	}
}

MEFconts AlgorithmSpecificFactory::getMEFconts() {
	if ( &mefConts == NULL ) {
		if ( analysisName == "findPeaks" ) {
			*mefConts = (MEFconts)MEFconts( getMEFinfo(), I["bufferCapacity"] ); 
			return *mefConts;
		}
	} else {
		return *mefConts;
	}
}

MEFinfo AlgorithmSpecificFactory::getMEFinfo() {
	if ( &info == NULL ) {
		if ( analysisName == "findPeaks" ) {
			*info = MEFinfo( S["filename"], S["password"] );
			return *info;
		}
	} else {
		return *info;
	}
}

DatabaseAccessor AlgorithmSpecificFactory::getDatabaseAccessor() {
	if ( &dba == NULL ) {
		if ( analysisName == "findPeaks" ) {
			*dba = DatabaseAccessor( S["databaseName"] );
			return *dba;
		}
	} else {
		return *dba;
	}
}


#include <iostream>
#include <vector>
#include <mysql/mysql.h>
#include <mysqlx/xdevapi.h>
#include "DatabaseAccessor.h"

#include "AlgorithmSpecificFactory.h"

using namespace std;

AlgorithmSpecificFactory::AlgorithmSpecificFactory() {
	
}

void AlgorithmSpecificFactory::set( string key, string value ) {
	S[ key ] = value;
}

void AlgorithmSpecificFactory::set( string key, double value ) {
	D[ key ] = value;
}

void AlgorithmSpecificFactory::set( string key, int value ) {
	I[ key ] = value;
}

void AlgorithmSpecificFactory::set( string key, long long value ) {
	LL[ key ] = value;
}

void setAnalysisName( string n ) {
	analysisName = "findPeaks";
}

CircularBuffer AlgorithmSpecificFactory::getCircularBuffer() {
	if ( circbuf == null ) {
		if ( analysisName == "findPeaks" )	
			circbuf = CircularBufferMEF_allPeaks( I["bufferCapacity"], LL["startTime"], LL["stepTime"] );
			return circbuf;
		}
	} else {
		return circbuf;
	}
}

MEFcont AlgorithmSpecificFactory::getMEFcont() {
	if ( mefConts == null ) {
		if ( analysisName == "findPeaks" ) {
			mefConts = mefConts( getMEFinfo(), I["bufferCapacity"] ); 
			return mefConts;
		}
	} else {
		return mefConts;
	}
}

MEFinfo AlgorithmSpecificFactory::getMEFinfo() {
	if ( info == null ) {
		if ( analysisName == "findPeaks" ) {
			info = MEFinfo( S["filename"], S["password"] );
			return( info );
		}
	} else {
		return info;
	}
}

DatabaseAccessor AlgorithmSpecificFactory::getDatabaseAccessor() {
	if ( dba == null ) {
		if ( analysisName = "findPeaks" ) {
			dba = DatabaseAccessor( S["databaseName"] );
			return dba;
		}
	} else {
		return dba;
	}
}


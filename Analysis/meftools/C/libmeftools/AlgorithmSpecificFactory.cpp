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
	cout << "Getting CircularBuffer." << endl;
	if ( circbuf == NULL ) {
		cout << "analysisName=" << analysisName << endl;
		if ( analysisName == "findPeaks" ) {
			cout << I["bufferCapacity"] << endl;
			cout << LL["startTime"] << endl;
			cout << LL["stepTime"] << endl;
			CircularBufferMEF_allPeaks tmp = CircularBufferMEF_allPeaks( I["bufferCapacity"], LL["startTime"], LL["stepTime"] );
			cout << "Made and returned." << endl;
			circbuf = &tmp;
			cout << "Returning new value." << endl;
			return *circbuf;
		}
	} else {
		cout << "Returning prior value." << endl;
		return *circbuf;
	}
}

MEFconts AlgorithmSpecificFactory::getMEFconts() {
	cout << "Getting MEFconts." << endl;
	if ( mefConts == NULL ) {
		if ( analysisName == "findPeaks" ) {
			cout << I["bufferCapacity"] << endl;
			MEFinfo tmpInfo = getMEFinfo();
			cout << "Got MEFinfo." << endl;
			MEFconts tmp = (MEFconts)MEFconts( tmpInfo, I["bufferCapacity"] ); 
			mefConts = &tmp;
			return *mefConts;
		}
	} else {
		return *mefConts;
	}
}

MEFinfo AlgorithmSpecificFactory::getMEFinfo() {
	if ( info == NULL ) {
		if ( analysisName == "findPeaks" ) {
			MEFinfo tmp = MEFinfo( S["filename"], S["password"] );
			cout << "MEFinfo assigned." << endl;
			info = &tmp;
			cout << "MEFinfo pointer assigned." << endl;
			return *info;
		}
	} else {
		return *info;
	}
}

DatabaseAccessor AlgorithmSpecificFactory::getDatabaseAccessor() {
	if ( dba == NULL ) {
		if ( analysisName == "findPeaks" ) {
			DatabaseAccessor tmp = DatabaseAccessor( S["databaseName"] );
			dba = &tmp;
			return *dba;
		}
	} else {
		return *dba;
	}
}


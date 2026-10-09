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

CircularBuffer AlgorithmSpecificFactory::getCircularBuffer() {
	if ( analysisName == "findPeaks" )	
		return CircularBufferMEF_allPeaks( I["bufferCapacity"], LL["startTime"], LL["stepTime"] );
}

MEFcont AlgorithmSpecificFactory::getMEFcont() {

}

MEFinfo AlgorithmSpecificFactory::getMEFinfo() {

}

DatabaseAccessor AlgorithmSpecificFactory::getDatabaseAccessor() {

}


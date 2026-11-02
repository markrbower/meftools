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

string get( string key ) {
	if ( S.find(key) ) {
		return S[ key ];
	} else if ( D.find(key) ) {
		return to_string( D[ key ] );
	} else if ( I.find(key) ) {
		return to_string( I[ key ] );
	} else if ( LL.find(key) ) {
		return to_string( LL[key] );
	} else {
		cout << "AlgorithmSpecificFactory failed to find key: " << key << endl;
		return "";
	}
}

map<string,string> prepareFor( string tablename, map<string,string> inputs ) {
	static map<string,string> ids;
	DatabaseAccessor dba = getDatabaseAccessor();
	map<string,string> values;
	// subjects
	values.insert( "name", inputs["subject"] );
	values.insert( "species", inputs["species"] );
	dba.write( "subjects", values );
	string dbIDsubject = dba.getPreviousID();
	ids.insert( "dbIDsubject" ) = dbIDsubject;
	values.clear();
	// experiments
	values.insert( "name", inputs["experiment"] );
	dba.write( "experiments", values );
	string dbIDexperiments = dba.getPreviousID();
	ids.insert( "dbIDexperiment" ) = dbIDexperiment;
	values.clear();
	// sources
	values.insert( "name", inputs["source"] );
	values.insert( "dbIDsubject", dbIDsubject );
	dba.write( "sources", values );
	string dbIDsource = dba.getPreviousID();
	ids.insert( "dbIDsource" ) = dbIDsource;
	values.clear();
	// collections
	values.insert( "name", inputs["collection"] );
	values.insert( "dbIDsubject", dbIDsubject );	
	values.insert( "dbIDexperiment", dbIDexperiment );
	values.insert( "date", inputs["date"] );
	values.insert( "place", inputs["place"] );
	dba.write( "collections", values );
	string dbIDcollection = dba.getPreviousID();
	ids.insert( "dbIDcollection" ) = dbIDcollection;
	values.clear();
	return ids;
}

CircularBuffer* AlgorithmSpecificFactory::getCircularBuffer() {
	cout << "Getting CircularBuffer." << endl;
	if ( circbuf == NULL ) {
		cout << "analysisName=" << analysisName << endl;
		if ( analysisName == "findPeaks" ) {
			cout << I["bufferCapacity"] << endl;
			cout << LL["startTime"] << endl;
			cout << LL["stepTime"] << endl;
			static CircularBufferMEF_allPeaks tmp = CircularBufferMEF_allPeaks( I["bufferCapacity"], LL["startTime"], LL["stepTime"] );
			cout << "Made and returned." << endl;
			circbuf = &tmp;
			cout << "Returning new value." << endl;
			return circbuf;
		}
	} else {
		cout << "Returning prior value." << endl;
		return circbuf;
	}
}

MEFconts* AlgorithmSpecificFactory::getMEFconts() {
	cout << "Getting MEFconts." << endl;
	if ( mefConts == NULL ) {
		cout << "Creating a new MEFconts." << endl;
		if ( analysisName == "findPeaks" ) {
			cout << I["bufferCapacity"] << endl;
			MEFinfo* tmpInfo = getMEFinfo();
			cout << "Got MEFinfo." << endl;
//			cout << "MEFconts: discontinuities size: " << tmpInfo->getDiscontinuities().size() << endl;
			static MEFconts tmp = (MEFconts)MEFconts( tmpInfo, I["bufferCapacity"] ); 
			mefConts = &tmp;
			return mefConts;
		}
	} else {
		cout << "Returning an existing MEFconts." << endl;
		return mefConts;
	}
}

MEFinfo* AlgorithmSpecificFactory::getMEFinfo() {
	cout << "Getting MEFinfo." << endl;
	if ( info == NULL ) {
		cout << "Creating a new MEFinfo." << endl;
		if ( analysisName == "findPeaks" ) {
			cout << "Assigning MEFinfo." << endl;
			static MEFinfo tmp = MEFinfo( S["filename"], S["password"] );
			info = &tmp;
			cout << "MEFinfo assigned." << endl;
			cout << "MEFinfo pointer assigned." << endl;
			cout << "discontinuities size: " << info->getDiscontinuities().size() << endl;
			return (info);
		}
	} else {
		cout << "Returning an existing MEFinfo." << endl;
		cout << "discontinuities size: " << info->getDiscontinuities().size() << endl;
		return (this->info);
	}
}

DatabaseAccessor* AlgorithmSpecificFactory::getDatabaseAccessor() {
	if ( dba == NULL ) {
		if ( analysisName == "findPeaks" ) {
			static DatabaseAccessor tmp = DatabaseAccessor( S["databaseName"] );
			dba = &tmp;
			return dba;
		}
	} else {
		return dba;
	}
}

void AlgorithmSpecificFactory::prepareForEvents( map<string,string> entries ) {

}

void AlgorithmSpecificFactory::prepareForMetrics( map<string,string> entries ) {

}


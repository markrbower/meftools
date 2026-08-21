/* analysisFindPeaks.cpp

Implements MEFanalysis interface to find peaks in MEF data.

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
#include "CircularBufferMEF.h"
#include "CircularBufferMEF_allPeaks.h"
#include "analysisFindPeaks.h"
#include "DatabaseAccessor.h"
#include <kfr/base.hpp>
#include <kfr/dft.hpp>
#include <kfr/dsp/iir_design.hpp>
#include <kfr/io/python_plot.hpp>


using namespace std;

void createAnalysisDatabase( const char* name ) {
	char queryStr[256];

	DatabaseAccessor dba_tmp = DatabaseAccessor( "mysql" ); // Generic db that should exist.
    	sprintf( queryStr, "create database if not exist %s;", name );
	dba_tmp.~DatabaseAccessor(); 

	DatabaseAccessor dba = DatabaseAccessor( name ); // Generic db that should exist.

//    	sprintf( queryStr, "use %s;", name );
//    	dba.runSQL( queryStr );

	cout << "Create Subjects table" << endl;
	// subject		dbIDsubject,        name, species
    	sprintf( queryStr, "drop table if exists subjects;" );
    	dba.runSQL( queryStr );
	cout << "creating subjects table" << endl;
    	dba.runSQL("create table subjects (dbIDsubject binary(16), name varchar(64), species varchar(64));" );
	cout << "subjects table created" << endl;

	cout << "Create Collections table" << endl;
	// collection	dbIDcollection,    name, dbIDsubject , date, place, task
    	sprintf( queryStr, "drop table if exists collections;" );
    	dba.runSQL( queryStr );
    	dba.runSQL("create table collections (dbIDcolletions binary(16), name varchar(64), dbIDsubject binary(16), date DATE, place varchar(64), task varchar(64) );" );

	cout << "Database tables created." << endl;
}


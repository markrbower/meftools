/* createAnalysisDatabase.cpp

Create all of the database tables required to pre-process EEG data.

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
    	sprintf( queryStr, "create database if not exists %s;", name );
	cout << queryStr << endl;
	dba_tmp.runSQL( queryStr );
	dba_tmp.~DatabaseAccessor(); 

	DatabaseAccessor dba = DatabaseAccessor( name ); // Generic db that should exist.

	cout << "Create Subjects table" << endl;
	// subject		dbIDsubject,        name, species
    	sprintf( queryStr, "drop table if exists subjects;" );
    	dba.runSQL( queryStr );
	cout << "creating subjects table" << endl;
    	dba.runSQL("create table subjects (uuid binary(16) DEFAULT (UUID_TO_BIN(UUID())) PRIMARY KEY, name varchar(64), species varchar(64), created_at TIMESTAMP(6) DEFAULT CURRENT_TIMESTAMP(6));" );
	cout << "subjects table created" << endl;

	cout << "Create Sources table" << endl;
	// source		dbIDsource	type
    	sprintf( queryStr, "drop table if exists sources;" );
    	dba.runSQL( queryStr );
	cout << "creating subjects table" << endl;
    	dba.runSQL("create table sources (uuid binary(16) DEFAULT (UUID_TO_BIN(UUID())) PRIMARY KEY, name varchar(64), dbIDsubject binary(16), type varchar(32), created_at TIMESTAMP(6) DEFAULT CURRENT_TIMESTAMP(6));" );
	cout << "subjects table created" << endl;

	cout << "Create Experiments table" << endl;
	// experiment	dbIDexperiment, name, dbIDcollection, dbIDanalysis
    	sprintf( queryStr, "drop table if exists experiments;" );
    	dba.runSQL( queryStr );
    	dba.runSQL("create table experiments (uuid binary(16) DEFAULT (UUID_TO_BIN(UUID())) PRIMARY KEY, name varchar(64), created_at TIMESTAMP(6) DEFAULT CURRENT_TIMESTAMP(6) );" );

	cout << "Create Analyses table" << endl;
	// analysis	dbIDanalysis,      name, description
    	sprintf( queryStr, "drop table if exists analyses;" );
    	dba.runSQL( queryStr );
    	dba.runSQL("create table analyses (uuid binary(16) DEFAULT (UUID_TO_BIN(UUID())) PRIMARY KEY, name varchar(64), description varchar(128), created_at TIMESTAMP(6) DEFAULT CURRENT_TIMESTAMP(6) );" );

	cout << "Create Collections table" << endl;
	// collection	dbIDcollection,    name, dbIDsubject , date, place, task
    	sprintf( queryStr, "drop table if exists collections;" );
    	dba.runSQL( queryStr );
    	dba.runSQL("create table collections (uuid binary(16) DEFAULT (UUID_TO_BIN(UUID())) PRIMARY KEY, name varchar(64), dbIDsubject binary(16), dbIDexperiment binary(16), date DATE, place varchar(64), created_at TIMESTAMP(6) DEFAULT CURRENT_TIMESTAMP(6) );" );

	cout << "Create Events table" << endl;
	// events		dbIDevents,        name, dbIDexperiment, time, data, label
    	sprintf( queryStr, "drop table if exists events;" );
    	dba.runSQL( queryStr );
    	dba.runSQL("create table events (uuid binary(16) DEFAULT (UUID_TO_BIN(UUID())) PRIMARY KEY, name varchar(64), dbIDsource binary(16), dbIDcollection binary(16), time BIGINT, data varchar(256), label varchar(32), created_at TIMESTAMP(6) DEFAULT CURRENT_TIMESTAMP(6) );" );

	cout << "Create metrics table" << endl;
	// metrics		dbIDmetrics,       name, dbIDevents, value
    	sprintf( queryStr, "drop table if exists metrics;" );
    	dba.runSQL( queryStr );
    	dba.runSQL("create table metrics (uuid binary(16) DEFAULT (UUID_TO_BIN(UUID())) PRIMARY KEY, name varchar(64), dbID1 binary(16), table1 varchar(32), dbID2 binary(16), table2 varchar(32), value varchar(64), label varchar(128), created_at TIMESTAMP(6) DEFAULT CURRENT_TIMESTAMP(6) );" );

	cout << "Database tables created." << endl;
}


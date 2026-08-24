/* main_0_createAnalysisDatabase
Mark Bower
Yale University

Compilation:
make main_0

Usage:
main_0 findPeaks

Result:
Creates a MySQL database for EEG data analysis

*/
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "DatabaseAccessor.h"

using namespace std;

void createAnalysisDatabase( const char* name );

int main(int argc, const char * argv[]) {
	char queryStr[128];
	const char* name = argv[1];
	cout << argv[0] << "\t" << argv[1] << endl;

	createAnalysisDatabase( name );

	cout << "Begin testing" << endl;
	DatabaseAccessor dba = DatabaseAccessor( name );
	// Test writing
	if ( dba.write( {"subjects","testSubject","testSpecies"} ) ) {
                cout << "Failure on \'Test writing subjects\'." << endl;
                return 0;
        }

	// Test reading
	string dbIDsubject = dba.readID( "select dbIDsubject from subjects where name=\'testSubject\';" );
	cout << "dbIDsubject: " << dbIDsubject << endl;





/*
	// Test linkage across tables
	//⁃	collection	dbIDcollection,    name, dbIDsubject , date, place, task
    	sprintf( queryStr, "insert into collections values (UUID_TO_BIN(UUID()),\'testColl\', dbIDsubject, \'2026-08-20\', \'testPlace\', \'Figure-8\');" );
	UUID dbIDcollection;
	if ( !dba.runSQL( queryStr ) ) {
		cout << "Failure on \'Test writing collection\'." << endl;
		return 0;
	}

	//⁃	analysis	dbIDanalysis,      name, description
    	sprintf( queryStr, "insert into analyses values (UUID_TO_BIN(UUID()),\'testName\', \'peak finding\');" );
	if ( !dba.runSQL( queryStr ) ) {
		cout << "Failure on \'Test writing collection\'." << endl;
		return 0;
	}

	//⁃	experiment	dbIDexperiment, name, dbIDcollection, dbIDanalysis
    	sprintf( queryStr, "insert into experiments values (UUID_TO_BIN(UUID()),\'testXp\', dbIDcollection, dbIDanalysis );" );
	if ( !dba.runSQL( queryStr ) ) {
		cout << "Failure on \'Test writing collection\'." << endl;
		return 0;
	}
*/




}

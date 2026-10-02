/* main_0_createAnalysisDatabase
Mark Bower
Yale University

Compilation:
make main_0

Usage:
main_0 test

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

	// Test writing: Subjects
	map<string,string> insertThese;
	insertThese["name"] = "testSubject";
	insertThese["species"] = "testSpecies";
	if ( !dba.write( "subjects", insertThese ) ) {
                cout << "Failure on \'Test writing subjects\'." << endl;
                return 0;
        }
	// Test reading
	string dbIDsubject = dba.getPreviousID( "subjects" );
	if ( dbIDsubject.empty() ) {
                cout << "Failure on \'Test reading subjects\'." << endl;
                return 0;
        }
	cout << "dbIDsubject: " << dbIDsubject << endl;
	dba.reset();

	// Experiments
	insertThese.clear();
	insertThese["name"] = "testXp";
	if ( !dba.write( "experiments", insertThese ) ) {
                cout << "Failure on \'Test writing experiments\'." << endl;
                return 0;
        }
	string dbIDexperiment = dba.getPreviousID( "experiments" );
	dba.reset();

	// Analyses
	insertThese.clear();
	insertThese["name"] = "testAnalysisName";
	insertThese["description"] = "peak finding";
	if ( !dba.write( "analyses", insertThese ) ) {
                cout << "Failure on \'Test writing analyses\'." << endl;
                return 0;
        }
	string dbIDanalysis = dba.getPreviousID( "analyses" );
	dba.reset();

	// Sources
	insertThese.clear();
	insertThese["name"] = "CSC01";
	insertThese["dbIDsubject"] = dbIDsubject;
	if ( !dba.write( "sources", insertThese ) ) {
                cout << "Failure on \'Test writing analyses\'." << endl;
                return 0;
        }
	string dbIDsource = dba.getPreviousID( "sources" );
	dba.reset();

	// Collections
	insertThese.clear();
	insertThese["name"] = "testCollection";
	insertThese["dbIDsubject"] = dbIDsubject;
	insertThese["dbIDexperiment"] = dbIDexperiment;
	insertThese["date"] = "2026-08-20";
	insertThese["place"] = "testPlace";
	if ( !dba.write( "collections", insertThese ) ) {
                cout << "Failure on \'Test writing collections\'." << endl;
                return 0;
        }
	string dbIDcollection = dba.getPreviousID( "collections" );
	dba.reset();

	// Persist several: Events
	insertThese.clear();
	insertThese["name"] = "testEvent";
	insertThese["dbIDsource"] = dbIDsource;
	insertThese["dbIDcollection"] = dbIDcollection;
	insertThese["time"] = "123456789012345";
	insertThese["data"] = "1.0,5.0,10.0,2.0,-5.0,0.0";
	insertThese["label"] = "AP";
	if ( !dba.write( "events", insertThese ) ) {
                cout << "Failure on \'Test writing experiments\'." << endl;
                return 0;
        }
	string dbIDevent1 = dba.getPreviousID( "events" );
	cout << "getPreviousID 1: " << dbIDevent1 << endl;

	insertThese["time"] = "123456789012346";
	insertThese["data"] = "1.0,5.0,-10.0,-2.0,-5.0,0.0";
	insertThese["label"] = "AP";
	if ( !dba.write( "events", insertThese ) ) {
                cout << "Failure on \'Test writing experiments\'." << endl;
                return 0;
        }
	string dbIDevent2 = dba.getPreviousID( "events" );
	cout << "getPreviousID 2: " << dbIDevent2 << endl;
	dba.reset();

	// Metrics
	insertThese.clear();
	insertThese["name"] = "testLink";
	insertThese["dbID1"] = dbIDevent1;
	insertThese["table1"] = "events";
	insertThese["dbID2"] = dbIDevent2;
	insertThese["table2"] = "events";
	insertThese["value"] = "0.5";
	insertThese["label"] = "CC";
	if ( !dba.write( "metrics", insertThese ) ) {
                cout << "Failure on \'Test writing experiments\'." << endl;
                return 0;
        }
	string dbIDmetric = dba.getPreviousID( "metrics" );
	dba.reset();

	cout << "Success!" << endl;
}

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
	map<string,string> insertThese;
	insertThese["name"] = "testSubject";
	insertThese["species"] = "testSpecies";
	if ( dba.write( "subjects", insertThese ) ) {
                cout << "Failure on \'Test writing subjects\'." << endl;
                return 0;
        }
	// Test reading
	string dbIDsubject = dba.getPreviousID();
	if ( dbIDsubject == NULL ) {
                cout << "Failure on \'Test reading subjects\'." << endl;
                return 0;
        }
	cout << "dbIDsubject: " << dbIDsubject << endl;

	// Test sequential db writing
	insertThese.clear();
	insertThese["name"] = "testCollection";
	insertThese["dbIDsubject"] = dbIDsubject;
	insertThese["date"] = "2026-08-20";
	insertThese["place"] = "testPlace";
	insertThese["task"] = "Figure-8";
	if ( dba.write( "collections", insertThese ) ) {
                cout << "Failure on \'Test writing collections\'." << endl;
                return 0;
        }
	string dbIDcollection = dba.getPreviousID();

	insertThese.clear();
	insertThese["name"] = "testAnalysisName";
	insertThese["description"] = "peak finding";
	if ( dba.write( "analyses", insertThese ) ) {
                cout << "Failure on \'Test writing analyses\'." << endl;
                return 0;
        }
	string dbIDanalysis = dba.getPreviousID();

	insertThese.clear();
	insertThese["name"] = "testXp";
	insertThese["dbIDcolletion"] = dbIDcollection;
	insertThese["dbIDanalysis"] = dbIDanalysis;
	if ( dba.write( "experiments", insertThese ) ) {
                cout << "Failure on \'Test writing experiments\'." << endl;
                return 0;
        }
	string dbIDexperiment = dba.getPreviousID();

	insertThese.clear();
	insertThese["name"] = "testEvent";
	insertThese["dbIDexperiment"] = dbIDexperiment;
	insertThese["time"] = "123456789012345";
	insertThese["data"] = "1.0,5.0,10.0,2.0,-5.0,0.0";
	insertThese["label"] = "AP";
	if ( dba.write( "events", insertThese ) ) {
                cout << "Failure on \'Test writing experiments\'." << endl;
                return 0;
        }
	string dbIDevent1 = dba.getPreviousID();

	insertThese["time"] = "123456789012346";
	insertThese["data"] = "1.0,5.0,-10.0,-2.0,-5.0,0.0";
	insertThese["label"] = "AP";
	if ( dba.write( "events", insertThese ) ) {
                cout << "Failure on \'Test writing experiments\'." << endl;
                return 0;
        }
	string dbIDevent2 = dba.getPreviousID();

	insertThese.clear();
	insertThese["name"] = "testLink";
	insertThese["dbIDevent1"] = dbIDevent1;
	insertThese["dbIDevent2"] = dbIDevent2;
	insertThese["data"] = 0.5;
	insertThese["label"] = "CC";
	if ( dba.write( "metrics", insertThese ) ) {
                cout << "Failure on \'Test writing experiments\'." << endl;
                return 0;
        }
	string dbIDmetric = dba.getPreviousID();

	cout << "Success!" << endl;
}

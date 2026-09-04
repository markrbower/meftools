#include <string>
#include <iostream>
#include <mysql/mysql.h>
#include <mysqlx/xdevapi.h>
#include "DatabaseAccessor.h"

using namespace std;

int main() {
//    DatabaseAccessor da("localhost", 33060, "root", "", string database_name);
    DatabaseAccessor da( "peaks" );

    cout << "Running" << endl;

    da.runSQL("create database if not exists peaks;");
    da.runSQL("use peaks;");

    da.runSQL("drop table if exists peaks;");

    da.runSQL("create table peaks (subject varchar(64), session varchar(64), time bigint, peakValue double, waveform varchar(256));" );

    map<long long,map<string,string>> peaks;
    map<string,string> map1;
    map1["waveform"] = "1,2,3,4,5";
    map1["peakValue"] = "5";
    peaks[10] = map1;
    map<string,string> map2;
    map2["waveform"] = "1,2,3,4,6";
    map2["peakValue"] = "6";
    peaks[12] = map2;

    map<string,string> fixed;
    fixed["subject"] = "subject_name";
    fixed["session"] = "session_nbr";
    da.mapInsert( "peaks", fixed, peaks );

    // Test the PreparedStatementBuilder by testing da.write( string tableName, list<map<string,string>> writeThese ) ...
    map<string,string> oneEntry;
    oneEntry["subject"] = "subject_name_2"; 
    oneEntry["session"] = "sesion_nbr_2"; 
    oneEntry["time"] = "3";
    oneEntry["peakValue"] = "6";
    oneEntry["waveform"] = "2,3,4,5,6";
    map<string,string> anotherEntry;
    anotherEntry["subject"] = "subject_name_2"; 
    anotherEntry["session"] = "sesion_nbr_2"; 
    anotherEntry["time"] = "4";
    anotherEntry["peakValue"] = "7";
    anotherEntry["waveform"] = "2,3,4,5,7";

    list<map<string,string>> oneList;
    oneList.push_back( oneEntry );
    oneList.push_back( anotherEntry );
    da.write( "peaks", oneList );

    MYSQL_ROW row;
    MYSQL_RES* result3 = da.runQuery("SELECT * FROM peaks;");

    cout << "id\t  Name\t\t    email\t\tage" << endl;
    cout << "---------------------------------------------------------------" << endl;
    while ((row = mysql_fetch_row(result3))) {
        cout << row[0] << "\t";
        cout << row[1] << "\t";
        cout << row[2] << "\t";
        cout << row[3] << endl;
    }


    return 0;
}
 

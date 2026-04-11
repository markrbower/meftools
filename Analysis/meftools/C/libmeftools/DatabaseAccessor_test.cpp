#include <string>
#include <iostream>
#include <mysql/mysql.h>
#include <mysqlx/xdevapi.h>
#include "DatabaseAccessor.h"

using namespace std;

int main() {
//    DatabaseAccessor da("localhost", 33060, "root", "", string database_name);
    DatabaseAccessor da( "peaks" );
    map<long long,string> peaks;

    cout << "Running" << endl;

    da.runSQL("create database if not exists peaks;");
    da.runSQL("use peaks;");

    da.runSQL("drop table if exists peaks;");

    da.runSQL("create table peaks (subject varchar(64), session varchar(64), time bigint, waveform varchar(256));" );

    peaks[1] = "1,2,3,4,5";
    peaks[2] = "1,2,3,4,6";

    da.vectorInsert( peaks );

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
 

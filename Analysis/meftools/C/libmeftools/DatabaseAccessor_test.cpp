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

    da.runQuery("create database if not exists peaks;");
    da.runQuery("use peaks;");

    mysqlx::SqlResult result0 = da.runQuery("drop table if exists peaks;");

//    mysqlx::SqlResult result1 = da.runQuery("create table peaks (subject varchar(64), session varchar(64), time bigint, waveform varchar(256));" );
    mysqlx::SqlResult result1 = da.runQuery("create table peaks (id int);" );

//    mysqlx::SqlResult result2 = da.runQuery("insert into peaks (subject,session,time,waveform) values (\'001\',\'day1\',12345,\'1,2,3,4,5\'),(\'001\',\'day1\',12346,\'1,2,3,4,6\');" );

    peaks[1] = "1,2,3,4,5";
    peaks[2] = "1,2,3,4,6";

    da.vectorInsert( peaks );

    mysqlx::SqlResult result3 = da.runQuery("SELECT * FROM peaks;");

    mysqlx::Row row;
    cout << "id\t  Name\t\t    email\t\tage" << endl;
    cout << "---------------------------------------------------------------" << endl;
    while ((row = result3.fetchOne())) {
        cout << row[0] << "\t";
        cout << row[1] << "\t";
        cout << row[2] << " \t";
        cout << row[3] << endl;
    }


    return 0;
}
 

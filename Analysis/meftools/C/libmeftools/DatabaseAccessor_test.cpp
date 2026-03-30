#include <string>
#include <iostream>
#include <mysqlx/xdevapi.h>
#include "DatabaseAccessor.h"

using namespace std;

int main() {
    DatabaseAccessor db("localhost", 33060, "root", "", "cpp_mysql_demo");

    mysqlx::SqlResult result0 = db.executeQuery("drop table if exists peaks;");

    mysqlx::SqlResult result1 = db.executeQuery("create table peaks (subject varchar(64), session varchar(64), time bigint, waveform varchar(256));" );

    mysqlx::SqlResult result2 = db.executeQuery("insert into peaks (subject,session,time,waveform) values (\'001\',\'day1\',12345,\'1,2,3,4,5\'),(\'001\',\'day1\',12346,\'1,2,3,4,6\');" );

    mysqlx::SqlResult result3 = db.executeQuery("SELECT * FROM peaks;");

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
 

#include <string>
#include <iostream>
#include <mysqlx/xdevapi.h>
#include "DatabaseAccessor.h"

using namespace std;

int main() {
    DatabaseAccessor db("localhost", 33060, "root", "", "cpp_mysql_demo");
    mysqlx::SqlResult result = db.executeQuery("SELECT * FROM users");

    mysqlx::Row row;
    cout << "id\t  Name\t\t    email\t\tage" << endl;
    cout << "---------------------------------------------------------------" << endl;
    while ((row = result.fetchOne())) {
        cout << row[0] << "\t";
        cout << row[1] << "\t";
        cout << row[2] << " \t";
        cout << row[3] << endl;
    }


    return 0;
}
 

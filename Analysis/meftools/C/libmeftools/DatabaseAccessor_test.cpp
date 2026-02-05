
#include <string>
#include <iostream>
#include <mysqlx/xdevapi.h>
#include "DatabaseAccessor.h"

int main() {
    DatabaseAccessor db("localhost", 33060, "root", "", "cpp_mysql_demo");
    db.executeQuery("SELECT * FROM users");
    return 0;
}
 

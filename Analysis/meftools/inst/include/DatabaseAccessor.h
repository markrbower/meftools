#ifndef DATABASE_ACCESSOR
#define DATABASE_ACCESSOR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>
#include <list>

#include <mysqlx/xdevapi.h>

using namespace std;

class DatabaseAccessor {

public:
    mysqlx::Session session;

    DatabaseAccessor(const std::string& uri) : session(mysqlx::Session(uri)) {}
    
    void close() {
        session.close();
    }

    mysqlx::SqlResult createTable(char& tableName,char& tableValues);

    bool runQuery( string tableName, string queryString );

};


#endif

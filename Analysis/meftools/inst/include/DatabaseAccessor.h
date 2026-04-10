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
#include <mysql/mysql.h>

using namespace std;

class DatabaseAccessor {

public:
    mysqlx::Session session;
    MYSQL *conn;

    DatabaseAccessor( char *dbname )
		: session( mysqlx::SessionOption::HOST, "localhost", 
		           mysqlx::SessionOption::PORT, 33060, 
		           mysqlx::SessionOption::USER, "root", 
		           mysqlx::SessionOption::PWD,  "",
		           mysqlx::SessionOption::CONNECT_TIMEOUT, 10 ) {
        cout << "In constructor" << endl;
	conn = mysql_init(NULL);
	if (conn == NULL) {
	    fprintf(stderr, "mysql_init() failed\n");
	}
        if (mysql_real_connect(conn, "localhost", "root", "", dbname, 33060, NULL, 0) == NULL) {
            fprintf(stderr, "mysql_real_connect() failed\n");
            mysql_close(conn);
        }	

        cout << "Exit constructor" << endl;
    }
    
    void close() {
        session.close();
    }

    mysqlx::SqlResult createTable(char& tableName,char& tableValues);

    mysqlx::SqlResult runQuery( string queryString );

    bool vectorInsert( map<long long,string> rows );

};


#endif

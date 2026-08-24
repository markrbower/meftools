#ifndef DATABASE_ACCESSOR
#define DATABASE_ACCESSOR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <map>
#include <string>
#include <list>
#include <iostream>

#include <mysqlx/xdevapi.h>
#include <mysql/mysql.h>

using namespace std;

class DatabaseAccessor {

public:
    MYSQL *conn;
    MYSQL_RES *result;
    string dbName;
    //kb::math::FiltFilt<double> filtfilt;

    DatabaseAccessor() {};

    DatabaseAccessor( string dbname ) {
        std::cout << "In constructor" << std::endl;
	conn = mysql_init(NULL);
	if (conn == NULL) {
	    fprintf(stderr, "mysql_init() failed\n");
	}
        if (mysql_real_connect(conn, "localhost", "root", "", dbname.c_str(), 33060, NULL, 0) == NULL) {
            fprintf(stderr, "mysql_real_connect() failed\n");
            mysql_close(conn);
        }	

        std::cout << "Exit constructor" << std::endl;
    }
/*
    ~DatabaseAccessor() {
	mysql_close(conn);
	cout << "DatabaseAccessor connection closed." << endl;
    }
*/   
    void close() {
        mysql_free_result( result );
        mysql_close( conn );
    }

    mysqlx::SqlResult createTable(string tableName,string tableValues);

    MYSQL_RES* runQuery( string queryString );

    bool runSQL( string queryString );

    bool mapInsert( string tableName, map<string,string> fixed, map<long long,map<string,string>> variables );

    bool write( std::initializer_list<string> args );

    string readID( string queryStr );    

};


#endif

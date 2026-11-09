#ifndef PREPARED_STATEMENT_BUILDER
#define PREPARED_STATEMENT_BUILDER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <vector>
#include <string>
#include <list>

using namespace std;

namespace psb {
class PreparedStatementBuilder {
    private:
	MYSQL_STMT *stmt;
        MYSQL_BIND *binding;
	int initialized;
	map<string,string> typeMap;
	string queryPrefix, queryMid, queryPostfix, query;
	int counter;

    public:
	PreparedStatementBuilder();

	PreparedStatementBuilder( MYSQL* conn, string tableName, map<string,string> insertThis, map<string,string> typeMap );

	PreparedStatementBuilder( MYSQL* conn, string tableName, list< map<string,string> > insertThese, map<string,string> typeMap );
	PreparedStatementBuilder( MYSQL* conn, string tableName, list< map<string,string> > insertThese, map<string,string> typeMap, list< map<string,string> > uniqueThese );

	int getInitialized();

	string getType( string colName ); 

	void clear();

	void generateQuery( string tableName, map<string,string> tmp );

	void generateQuery( string tableName, map<string,string> tmp, map<string,string> unique );

	void addEntry( string name, string value, void* up );

	MYSQL_STMT* generateStatement();

};
}
#endif


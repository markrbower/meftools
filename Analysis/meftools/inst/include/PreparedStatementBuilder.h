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
        MYSQL_BIND *bind_;
	int initialized;
	map<string,string> typeMap;
	string queryPrefix, queryPostfix, query;
	int counter;

    public:
	PreparedStatementBuilder();

	PreparedStatementBuilder( string tableName, map<string,string> insertThis, map<string,string> typeMap );

	PreparedStatementBuilder( string tableName, list< map<string,string> > insertThese, map<string,string> typeMap );

	int getInitialized();

	void clear();

	void addEntry( string name, string value );

	void persist();

};
}
#endif


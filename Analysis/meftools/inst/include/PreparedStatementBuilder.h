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

class PreparedStatementBuilder {
    private:
	MYSQL_STMT *stmt;
        MYSQL_BIND *bind_;
	int initialized;

    public:
	PreparedStatementBuilder();

	PreparedStatementBuilder( string tableName, map<string,string> insertThis );

	PreparedStatementBuilder( string tableName, list< map<string,string> > insertThese );

	int getInitialized();

	void clear();

};
#endif


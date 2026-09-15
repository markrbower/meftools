#include <iostream>
#include <vector>
#include <mysql/mysql.h>
#include <mysqlx/xdevapi.h>
#include "DatabaseAccessor.h"

#include "PreparedStatementBuilder.h"

using namespace std;
using namespace psb;

PreparedStatementBuilder::PreparedStatementBuilder() {
	initialized = 0;
}

PreparedStatementBuilder::PreparedStatementBuilder(MYSQL* conn, string tableName, map<string,string> insertThis, map<string,string> typeMap_ ) {
	initialized = 1;
	binding = (MYSQL_BIND*)malloc( insertThis.size() * sizeof(MYSQL_BIND) );
	stmt = mysql_stmt_init(conn);

	typeMap = typeMap_;

	generateQuery( tableName, insertThis );
}

PreparedStatementBuilder::PreparedStatementBuilder(MYSQL* conn, string tableName, list< map<string,string> > insertThese, map<string,string> typeMap_ ) {
	cout << "Entering PreparedStatementBuilder constructor." << endl;
	initialized = 1;
	binding = (MYSQL_BIND*)malloc( insertThese.front().size() * sizeof(MYSQL_BIND) );
	stmt = mysql_stmt_init(conn);
        
	typeMap = typeMap_;

	generateQuery( tableName, insertThese.front() );
	cout << "Leaving PreparedStatementBuilder constructor." << endl;
}

void PreparedStatementBuilder::generateQuery( string tableName, map<string,string> thisMap ) {
        queryPrefix = "INSERT INTO " + tableName + " \(";
        queryPostfix = "\) VALUES \(";

	int firstTimeFlag = 1;
	for ( auto const&[key,value] : thisMap ) {
		if ( firstTimeFlag == 0 ) {
			queryPrefix.append( "," );
			queryPostfix.append( "," );
		}
		firstTimeFlag = 0;
		queryPrefix.append( key );
		queryPostfix.append( "?" );
	}
	query = queryPrefix + queryPostfix + ");";
	cout << query << endl;

        unsigned long stmt_length = query.size();
        int status = mysql_stmt_prepare(stmt, query.c_str(), stmt_length );
        if (status) {
            cout << "Failed on prepare" << endl;
            cout << query << endl;
            fprintf(stderr, "Error: %s (errno: %d)\n", mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
            exit(1);
        } else {
                cout << "Statement looks good." << endl;
        }   
}

int PreparedStatementBuilder::getInitialized() {
	return PreparedStatementBuilder::initialized;
}

string PreparedStatementBuilder::getType( string colName ) {
	if( typeMap.find(colName) != typeMap.end() ) { // key found in map
		cout << "Column: " << colName << "\t" << typeMap[colName] << endl;
		return typeMap[colName];
	} else {
		cout << "PreparedStatementBuilder::getType :: type not found for " << colName << endl;
		return "";
	}
}

void PreparedStatementBuilder::clear() {
	counter = 0;
        memset(binding, 0, sizeof(binding));
}

void PreparedStatementBuilder::addEntry( string key, string value, void* up ) {
	unsigned long lenStr;
	char varcharValue[2048];
	unsigned long varcharLength;
	// Find the data type
	string datatype = getType( key );
	cout << "addEntry " << counter << "\t" << key << "\t" << value << "\t" << datatype << endl;

	// Call the appropriate add function
	if ( datatype == "varchar" ) {
		up = new char[ value.length() + 1 ];
		std::strcpy( (char*)up, value.c_str() );
                binding[counter].buffer_type = MYSQL_TYPE_STRING;
                binding[counter].buffer = (char *)up;
                binding[counter].buffer_length = strlen((const char*)up);
                binding[counter].is_null = 0;
	} else if ( datatype == "bigint" ) {
		up = malloc(sizeof(long long));
		long long* llp = static_cast<long long*>(up);
		*llp = std::stoll( value );
                binding[counter].buffer_type = MYSQL_TYPE_LONGLONG;
                binding[counter].buffer = llp;
                binding[counter].length = 0;
                binding[counter].is_null = 0;
	} else if ( datatype == "date" ) {
		up = malloc( sizeof(MYSQL_TIME) );
		MYSQL_TIME* ts = static_cast<MYSQL_TIME*>(up);

		string delimiter = "-";
		std::vector<std::string> tokens;
                size_t pos = 0;
                std::string token;
                while ((pos = value.find(delimiter)) != std::string::npos) {
                	token = value.substr(0, pos);
                	tokens.push_back(token);
                	value.erase(0, pos + delimiter.length());
    		}
    		tokens.push_back(value);
		ts->day = std::stoul( tokens[2] );
		ts->month = std::stoul( tokens[1] );
		ts->year = std::stoul( tokens[0] );

		binding[counter].buffer_type = MYSQL_TYPE_DATE;
		binding[counter].buffer = (char *)&ts;
		binding[counter].is_null = 0;
		binding[counter].length = 0;
	} else if ( datatype == "double" ) {
		up = malloc(sizeof(double));
		double* dp = static_cast<double*>(up);
		*dp = std::stod( value );
        	binding[counter].buffer_type = MYSQL_TYPE_DOUBLE;
                binding[counter].buffer = dp;
                binding[counter].length = 0;
                binding[counter].is_null = 0;
	} else {
		cout << "PreparedStatementBuilder: addEntry: unknown datatype" << endl;
	}
        counter++;
}

MYSQL_STMT* PreparedStatementBuilder::generateStatement() {
	cout << "Into PSB::generateStatement" << endl;
        int status;

	// How can I check that "stmt" exists and "binding" exists and has values?

	try {
        	status = mysql_stmt_bind_param(stmt, binding);
	} catch ( const char* msg ) {
		cout << "Error: " << msg;
	}
	cout << "Binding completed." << endl;
	if (status) {
              	fprintf(stderr, "Error: %s (errno: %d)\n", mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
               	exit(1);
        } else {
               	cout << "Binding looks good." << endl;
        }       
	return stmt;
}

/*
int PreparedStatementBuilder::() {
}

int PreparedStatementBuilder::() {
}
*/





// TESTING
/*
int main() {
    // 
    PreparedStatementBuilder builder();




}
*/

// iatabaseAccessor.cpp
// Mark R. Bower
// Yale University
/*
  #'  Organizes MySQL database activity. 
  #'    For general use, create a Singleton that allows users to set their defaults.
  #' 
*/
#include <iostream>
#include <mysql/mysql.h>
#include <mysqlx/xdevapi.h>
#include <string>
#include <map>

#include "PreparedStatementBuilder.h"
#include "DatabaseAccessor.h"

using namespace std;
using namespace psb;

mysqlx::SqlResult DatabaseAccessor::createTable( string tableName, string tableValues ) {

}

// runQuery checks for a valid result
MYSQL_RES* DatabaseAccessor::runQuery( string queryString ) {
	if (mysql_query(conn, queryString.c_str())) {
	    fprintf(stderr, "Query failed: %s\n", mysql_error(conn));
	    return NULL;
	}
	result = mysql_store_result(conn);
	if (result == NULL) {
	    fprintf(stderr, "Could not retrieve result set: %s\n", mysql_error(conn));
	    return NULL;
	}
	return( result );
}

// runSQL does not return a result
bool DatabaseAccessor::runSQL( string queryString ) {
	if (mysql_query(conn, queryString.c_str())) {
	    fprintf(stderr, "Query failed: %s\n", mysql_error(conn));
	    return 0;
	}
	return 1;
}

bool DatabaseAccessor::mapInsert( string tableName, map<string,string> fixed_values, map<long long,map<string,string>> variables ) {
	// Prepare the statement
	MYSQL_STMT *stmt;
	MYSQL_BIND bind[5];
	char *value0 = "subject";
	unsigned long length0 = strlen(value0);
	char *value1 = "example";
	unsigned long length1 = strlen(value1);
	long long value2 = 42;
	unsigned long length3;
	int status;
	char varcharValue[100];
	unsigned long varcharLength;

	cout << "Running" << endl;
	stmt = mysql_stmt_init(conn);
	if ( !stmt ) {
		cout << "Could not initialize statement." << endl;
	}
        cout << "Statement initialized" << endl;

	string query = "INSERT INTO peaks (subject,session,time,peakValue,waveform) VALUES (?,?,?,?,?);";
	unsigned long stmt_length = query.size();
	cout << query << endl;
	status = mysql_stmt_prepare(stmt, query.c_str(), stmt_length );
  	if (status) {
	    cout << "Failed on prepare" << endl;
	    cout << query << endl;
	    fprintf(stderr, "Error: %s (errno: %d)\n", mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
	    exit(1);
	} else {
		cout << "Statement looks good." << endl;
	}
	memset(bind, 0, sizeof(bind));

	// Start a transaction
	int fixedLength = fixed_values.size();
	
	cout << "FIXED VALUES" << endl;
	int count = 0;
	for ( auto element: fixed_values ) {
		cout << count << "\t" << element.first << "\t" << element.second << endl;
		bind[count].buffer_type = MYSQL_TYPE_STRING;
		string value = element.second;
		strcpy(varcharValue, value.c_str());
		varcharLength = strlen(varcharValue);
		bind[count].buffer = (char *)varcharValue;
		bind[count].buffer_length = sizeof(varcharValue);
		bind[count].length = &varcharLength;
		bind[count].is_null = 0;
		count++;
	}

	cout << "VARIABLES" << endl;
	runSQL( "START TRANSACTION;" );
	try {
		for ( auto const &[outer_key, inner_map] : variables ) {
			for ( auto const &[inner_key, inner_value] : inner_map ) {
                                if ( inner_key == "peakValue" ) {
                                        bind[3].buffer_type = MYSQL_TYPE_DOUBLE;
                                        double dvalue = std::stod( inner_value );
                                        bind[3].buffer = (char*)&dvalue;
                                        bind[3].length = 0;
                                        bind[3].is_null = 0;
				} else {
					bind[2].buffer_type = MYSQL_TYPE_LONGLONG;
					bind[2].buffer = (long long*)&outer_key;
					bind[2].length = 0;
					bind[2].is_null = 0;
		
					bind[4].buffer_type = MYSQL_TYPE_VARCHAR;
					bind[4].buffer = (char *)inner_value.c_str();
					length3 = strlen( inner_value.c_str() );
					bind[4].length = &length3;
					bind[4].is_null = 0;
					cout << count << ":\t" << outer_key << "\t" << inner_value << endl;
				}
			}
			status = mysql_stmt_bind_param(stmt, bind);
			if (status) {
			    fprintf(stderr, "Error: %s (errno: %d)\n", mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
			    exit(1);
			} else {
			    cout << "Binding looks good." << endl;
			}

			cout << "executing statement" << endl;
			status = mysql_stmt_execute(stmt);
			if (status) {
			    fprintf(stderr, "Error: %s (errno: %d)\n",
			            mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
			    exit(1);
			}
			cout << "executed statement" << endl;
		}
		// Commit the transaction if everything went well
		runSQL( "COMMIT;" );
	} catch (const mysqlx::Error &err) {
		// Rollback the transaction in case of an error
		runSQL( "ROLLBACK;" );
		cout << "rolled back" << endl;
	} 
	cout << "closing statement" << endl;
	if (mysql_stmt_close(stmt)) {
	  fprintf(stderr, " failed while closing the statement\n");
	  fprintf(stderr, " %s\n", mysql_error(conn));
	  exit(0);
	}
	cout << "exiting function" << endl;
}

bool DatabaseAccessor::write( string tableName, list<map<string,string>> insertThese ) {
	if ( !builder.getInitialized() ) {
	        cout << "Prepare the query and bind values at the same time." << endl;
		map<string,string> typeMap = getColumnTypes( tableName );
		builder = PreparedStatementBuilder( tableName, insertThese, typeMap );
	}

	int count=0;
	list<map<string,string>>::iterator it = insertThese.begin();
        // Iterate through the list
        while (it != insertThese.end()) {
		builder.clear();
		for ( const auto& [key,value] : *it ) {
			builder.addEntry( key, value );
		}
		persist( builder );
	}
	return 1;
}

string DatabaseAccessor::readID( string queryStr ) {
	// A special purpose read query for UUID values.
	string dbID;
        MYSQL_RES* result = runQuery( queryStr );
        if ( result == NULL ) {
                cout << "Failure on \'DatabaseAccessor reading a UUID\'." << endl;
                return 0;
        }
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result)) != NULL) {
            for (int i = 0; i < mysql_num_fields(result); i++) {
//                std::cout << row[i] << " ";
                dbID = row[i];
            }
            std::cout << std::endl;
        }
        mysql_free_result(result);
	if ( dbID.empty() ) {
                cout << "Failure on \'DatabaseAccessor reading a UUID. Nothing found.\'." << endl;
                return 0;
        }
	return dbID;
}

map<string,string> DatabaseAccessor::getColumnTypes( string tableName ) {
	map<string,string> typeMap; // Make the typeMap here to return for the PSB constructor.
        char queryStr1[128];
        sprintf( queryStr1,"SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name=\'%s\';", tableName.c_str() );
	cout << "Query #1: " << queryStr1 << endl;
        MYSQL_RES* result1 = runQuery( queryStr1 );
        MYSQL_ROW row1;
        while ((row1 = mysql_fetch_row(result1)) != NULL) {
                cout << row1[0] << endl;
                char queryStr2[128];
                sprintf( queryStr2,"SELECT DATA_TYPE FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name=\'%s\' AND COLUMN_NAME = \'%s\';", tableName.c_str(), row1[0] );
		cout << "Query #2: " << queryStr2 << endl;
                MYSQL_RES* result2 = runQuery( queryStr2 );
                MYSQL_ROW row2;
		cout << "DATATYPES" << endl;
                while ((row2 = mysql_fetch_row(result2)) != NULL ) {
                        typeMap[ row1[0] ] = row2[0];
			cout << row1[0] << "\t" << row2[0] << endl;
                }
        }
	cout << "Done with getColumnTypes" << endl;
}

void DatabaseAccessor::persist( PreparedStatementBuilder builder ) {
	stmt = builder.generateStatement();

        runSQL( "START TRANSACTION;" );
        try {
                cout << "executing statement" << endl;
                int status = mysql_stmt_execute(stmt);
                if (status) {
                        fprintf(stderr, "Error: %s (errno: %d)\n",
                                    mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
                        exit(1);
                }
                cout << "executed statement" << endl;
                runSQL( "COMMIT;" );
        } catch (const mysqlx::Error &err) {
                // Rollback the transaction in case of an error
                runSQL( "ROLLBACK;" );
                cout << "rolled back" << endl;
        }

}


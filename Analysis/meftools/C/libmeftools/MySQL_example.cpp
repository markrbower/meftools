#include <mysqlx/xdevapi.h>  
#include <iostream>  
using namespace std;  
using namespace mysqlx;  
 
int main() {  
    try {  
	cout << "Starting." << endl;
        // Connect to MySQL: Replace username/password with your credentials  
        Session session("localhost", 33060, "root", "", "cpp_mysql_demo");  
        cout << "Connected to MySQL successfully!" << endl;  
 
        // Execute SELECT query  
        RowResult result = session.sql("SELECT * FROM users").execute();  
 
        // Print column names  
        const Columns& cols = result.getColumns();  
        cout << "\nColumns:\n";  
        for (const auto& col : cols) {  
            cout << col.getColumnName() << "\t";  
        }  
        cout << "\n-------------------------\n";  
 
        // Print rows  
        for (const Row& row : result) {  
            cout << row[0] << "\t"    // Access by column name  
                 << row[1] << "\t"  // (or use row[0], row[1], etc.)  
                 << row[2] << "\t"  
                 << row[3] << endl;  
        }  
 
        // Close the session (optional; session destructor closes it automatically)  
        session.close();  
        cout << "\nSession closed." << endl;  
    }  
    catch (const Error& e) {  
        cerr << "Error: " << e.what() << endl;  
        return 1;  
    }  
    catch (const bad_alloc& e) {  
        cerr << "Memory allocation error: " << e.what() << endl;  
        return 1;  
    }  
    catch (...) {  
        cerr << "Unknown error occurred." << endl;  
        return 1;  
    }  
 
    return 0;  
} 

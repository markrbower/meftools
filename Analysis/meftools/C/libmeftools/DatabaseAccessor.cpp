// MEFcont.cpp
// Mark R. Bower
// Yale University
/*
  #'  Organizes MySQL database activity. 
  #' 
*/
#include <mysqlx/xdevapi.h>

class DatabaseAccessor {
private:
    std::unique_ptr<mysqlx::Session> _session;

public:
    DatabaseAccessor(const std::string& host, int port, const std::string& user, const std::string& password) {
        _session = std::make_unique<mysqlx::Session>(host, port, user, password);
    }

    ~DatabaseAccessor() {
        if (_session) {
            _session->close();
        }
    }

    void executeQuery(const std::string& query) {
        mysqlx::SqlStatement stmt = _session->sql(query);
        stmt.execute();
    }
};


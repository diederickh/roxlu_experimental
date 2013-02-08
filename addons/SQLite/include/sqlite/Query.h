#ifndef ROXLU_DATABASE_QUERYH
#define ROXLU_DATABASE_QUERYH

#include <sqlite3.h>
#include <string>

namespace roxlu {

  class Database;

  class QueryResult;

  class Query {
  public:
    Query(Database& db);
    Query(Database& db, std::string sql);
    Query& operator=(const Query& other);
    ~Query();
    Database& getDB();
    sqlite3* getSQLite();
    virtual bool execute(QueryResult& result);                  /* default implementation, used to execute raw queries */
  protected:
    Database& db;
    std::string sql;
  };

} // roxlu
#endif

#include <sqlite/Query.h>
#include <sqlite/Database.h>
#include <sqlite/QueryResult.h>

namespace roxlu {

  Query::Query(Database& db)
    :db(db)
  {
  }

  Query::Query(Database& db, std::string sql)
    :db(db)
    ,sql(sql)
  {
    
  }

  Query& Query::operator=(const Query& other) {
    db = other.db;
    sql = other.sql;
    return *this;
  }

  Query::~Query() {
  }

  Database& Query::getDB() {
    return db;
  }

  sqlite3* Query::getSQLite() {
    return db.getDB();
  }

  bool Query::execute(QueryResult& result) {
    return result.execute(sql);
  }

};

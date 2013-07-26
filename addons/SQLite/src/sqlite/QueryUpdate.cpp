#include <roxlu/core/Log.h>
#include <sqlite/QueryUpdate.h>
#include <sqlite/Database.h>

namespace roxlu {

  QueryUpdate::QueryUpdate(Database& db)
    :Query(db)
  {
  }
  
  QueryUpdate::QueryUpdate(Database& db, const string table) 
    :Query(db)
    ,table(table)
  {
  }

  QueryUpdate::QueryUpdate(const QueryUpdate& other) 
    :Query(other.db)
  {
    *this = other;
  }

  QueryUpdate& QueryUpdate::operator=(const QueryUpdate& other) {
    if(this == &other) {
      return *this;
    }

    table = other.table;
    field_values = other.field_values;
    return *this;
  }

  QueryUpdate::~QueryUpdate() {
    where_clause = "";
  }

  std::string QueryUpdate::toString() {
    std::string sql;
    std::string values;
    std::string fields;

    sql.append("update ");
    sql.append(table);

    const vector<QueryParam*>& params = field_values.getParams();

    if(params.size()) {
      size_t nfields = params.size();
      size_t count = 0;
      sql.append(" set ");

      for(std::vector<QueryParam*>::const_iterator it = params.begin(); it != params.end(); ++it) {
        QueryParam* p = *it;
        sql.append(p->getField());
        sql.append(" = ");
        sql.append(p->getBindSQL());
        if(count != (nfields - 1)) {
          sql.append(", ");
        }
      }
    }

    if(where_clause.size()) {
      sql.append(" where ");
      sql.append(where_clause);
    }
    
    //RX_VERBOSE("SQL: %s", sql.c_str());

    return sql;
  }

  bool QueryUpdate::execute() {

    std::string sql = toString();
    if(!sql.length()) {
      return false;
    }

    sqlite3_stmt* stmt;
    if(!getDB().prepare(sql, &stmt)) {
      sqlite3_finalize(stmt);
      return false;
    }

    if(!getDB().bind(field_values.getParams(), &stmt, Database::QUERY_UPDATE)) {
      sqlite3_finalize(stmt);
      return false;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE) {
      RX_ERROR("Error: %s", sqlite3_errmsg(getSQLite()));
      sqlite3_finalize(stmt);
      return false;
    }

    sqlite3_finalize(stmt);
    return true;
  }

}; // roxlu

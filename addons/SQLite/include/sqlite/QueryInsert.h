/* 

   # QueryInsert

   Used to generate insert queries; can also be used to UPSERT kind of queries (make sure
   to make your field unique).


   ````c++

    QueryResult qr(db);
    db.query("CREATE TABLE IF NOT EXISTS state("
             "   name TEXT UNIQUE, "
             "   value TEXT)").execute(qr);
    qr.finish();

    db.insert("state").use("name","refresh_token").use("value", 122).orReplace().execute();

   ````


 */
#ifndef ROXLU_DATABASE_QUERY_INSERTH
#define ROXLU_DATABASE_QUERY_INSERTH

#include <sqlite/Query.h>
#include <sqlite/QueryParams.h>

namespace roxlu {

  class Database;
  class QueryInsert : public Query {
  public: 
    QueryInsert(Database& db);
    QueryInsert(Database& db, const string& table);
    QueryInsert(const QueryInsert& other);
    QueryInsert& operator=(const QueryInsert& other);
    ~QueryInsert();	
	
    QueryInsert& setTable(const string& tableName);
	
    template<typename T>
      QueryInsert& use(const string& fieldName, const T& fieldValue) {
      field_values.use(fieldName, fieldValue);
      return *this;
    }
	
    QueryInsert& useTimestamp(const string& fieldName) {
      field_values.useTimestamp(fieldName);
      return *this;
    }
	
    QueryInsert& useDateTime(const string& fieldName) {
      field_values.useDateTime(fieldName);
      return *this;
    }
	
    // what on duplicate?
    QueryInsert& orRollback();
    QueryInsert& orAbort();
    QueryInsert& orReplace();
    QueryInsert& orIgnore();
    QueryInsert& orFail();

	
    string toString();
	
    bool execute();
  protected:
    string or_clause;
    string table;
    QueryParams field_values;
  };


  // or?.... what?
  // ------------------------------------------
  inline QueryInsert& QueryInsert::orRollback() {
    or_clause = " OR ROLLBACK ";
    return *this;
  }

  inline QueryInsert& QueryInsert::orAbort() {
    or_clause = " OR ABORT ";
    return *this;
  }

  inline QueryInsert& QueryInsert::orReplace() {
    or_clause = " OR REPLACE ";
    return *this;
  }

  inline QueryInsert& QueryInsert::orIgnore() {
    or_clause = " OR IGNORE ";
    return *this;
  }

  inline QueryInsert& QueryInsert::orFail() {
    or_clause = " OR FAIL ";
    return *this;
  }

} // roxlu
#endif

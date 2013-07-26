/*

  # QueryUpdate

  Very basic implementation for an update query.

  _Example_
  ````c++

  QueryResult qr(db);
  db.query("CREATE TABLE IF NOT EXISTS state("
           "   name TEXT UNIQUE, "
           "   value TEXT)").execute(qr);
  qr.finish();
  
  db.update("state").use("value", 1235).where("name", "refresh_token").execute();
  ````

 */

#ifndef ROXLU_DATABASE_QUERY_UPDATE
#define ROXLU_DATABASE_QUERY_UPDATE

#include <sqlite/Query.h>
#include <sqlite/QueryResult.h>
#include <sqlite/QueryParams.h>
#include <sstream>
#include <string>

namespace roxlu {

  class Database;
  
  class QueryUpdate : public Query {
  public:
    QueryUpdate(Database& db);
    QueryUpdate(Database& db, const string table);
    QueryUpdate(const QueryUpdate& other);
    QueryUpdate& operator=(const QueryUpdate& other);
    ~QueryUpdate();


    template<class T>
      QueryUpdate& where(std::string field, T value);

    template<class T>
      QueryUpdate& use(std::string field, T value);;

    bool execute();

    std::string toString();

  private:
    std::string table;
    std::string where_clause;
    QueryParams field_values;
  };

  template<class T>
   QueryUpdate&  QueryUpdate::where(std::string field, T value) {
    std::stringstream ss;
    ss << " " << field << " = \"" << value << "\" ";
    where_clause = ss.str();
    return *this;
  }

  template<class T>
    QueryUpdate& QueryUpdate::use(std::string field, T value) {
    field_values.use(field, value);
    return *this;
  }

}; // roxlu

#endif

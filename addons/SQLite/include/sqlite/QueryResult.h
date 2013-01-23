/**
 * Object used to iterate over query data.
 *  
 * IMPORTANT: make sure that 'free()' is called
 * to free up and finalize the sqlite3_stmt. We
 * will call free() in the destructor too. 
 *
 */
#ifndef ROXLU_DATABASE_QUERYRESULTH
#define ROXLU_DATABASE_QUERYRESULTH

#include <stdint.h>
#include <sqlite3.h>
#include <sqlite/QueryParams.h>

namespace roxlu {

  class Database;

  class QueryResult {
  public:
    QueryResult(Database& db);
    QueryResult(const QueryResult& other);
    QueryResult& operator=(const QueryResult& other);
    ~QueryResult();
	
    bool execute(const string& sql, QueryParams& params, int queryType);
    bool isOK();
    bool next();
    bool free();
	
    string getString(int index);
    int64_t getInt(int index);
    float getFloat(int index);
    bool isLast();
	
  private:
    Database& getDB();
    Database& db;
    sqlite3_stmt* stmt;
    bool is_ok;
    int row_index;
    int last_result;
  };

  inline bool QueryResult::isOK() {
    return is_ok;
  }

  inline Database& QueryResult::getDB() {
    return db;
  }

} // roxlu

#endif

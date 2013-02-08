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
#include <vector>
#include <string>

namespace roxlu {

  class Database;

  class QueryResult {
  public:
    QueryResult(Database& db);
    QueryResult(const QueryResult& other);
    QueryResult& operator=(const QueryResult& other);
    ~QueryResult();
	
    bool execute(const string& sql, QueryParams& params, int queryType);
    bool execute(const string& sql);                                         /* execute a raw query directly, don't forget to call finish() in case of a delete, insert or update  */
    bool next();
    bool free();
    bool finish();                                                           /* steps over all entries and finalizes the query. use this when you call: QueryResult.execute("update table set field = 0").finish() */
	
    string getString(int index);
    int64_t getInt(int index);
    float getFloat(int index);
    bool isLast();

    std::vector<std::string> getFieldNames();                                 /* returns an vector with the column/field names of the current statement */
	
  private:
    Database& getDB();
    Database& db;
    sqlite3_stmt* stmt;
    int row_index;
    int last_result;
  };

  inline Database& QueryResult::getDB() {
    return db;
  }

} // roxlu

#endif

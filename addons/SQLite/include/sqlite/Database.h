#ifndef ROXLU_DATABASEH
#define ROXLU_DATABASEH

#include <vector>
#include <string>
#include <sqlite3.h>
#include <sqlite/QueryInsert.h>
#include <sqlite/QueryUpdate.h>
#include <sqlite/QuerySelect.h>
#include <sqlite/QueryDelete.h>
#include <sqlite/QueryParam.h>
#include <sqlite/QueryParams.h>

using std::string;
using std::vector;

#define SQL(str) #str    // stringify macro: SQL(insert into ...)

int roxlu_database_busy_handler(void* v, int r);

namespace roxlu {

  class Database {
  public:
    enum QueryTypes {
      QUERY_INSERT
      ,QUERY_UPDATE
      ,QUERY_SELECT
      ,QUERY_DELETE
    };
	
    Database();
    ~Database();
    bool open(const string& fileName, bool datapath = false);
    Query query(const string& sql);
    int lastInsertID();
    bool beginTransaction();
    bool endTransaction();
    QueryInsert insert(const string& table);
    QueryUpdate update(const string& table);
    QuerySelect	select(const string& selectFields);
    QuerySelect select();
    QueryDelete remove();
    QueryDelete remove(const string& table);
	
    bool prepare(const string& sql, sqlite3_stmt** stmt);
    bool bind(const vector<QueryParam*>& params, sqlite3_stmt** stmt, int queryType);
    sqlite3* getDB();
    void printCompileInfo();
  private:
    string file;
    sqlite3* db;
    bool opened;
  };


  inline sqlite3* Database::getDB() {
    return db;
  }

  /*

    db.open("test.db");

    QueryResult result(db);

    db.query("CREATE TABLE IF NOT EXISTS scores (" \
    " id INTEGER PRIMARY KEY AUTOINCREMENT" \
    " ,time TEXT" \
    ", score INTEGER" \
    ", name TEXT "\
    ");").execute(result);
    result.finish();  // you need to finalize raw queries yourself
		
		
    db.insert("scores")
    .use("time", "now - canged")
    .use("score", 100)
    .use("name", "diederick")
    .execute();	
	
    db.insert("scores")
    .use("time", "now - canged")
    .use("score", 101)
    .use("name", "marin")
    .execute();	
	
    int num_rows = 0;
    for(int i = 0; i < num_rows; ++i) {
    db.insert("scores")
    .use("time", "now - canged")
    .use("score", 100)
    .use("name", "diederick")
    .execute();
    }
		
	
    QueryResult result(db);
    db.select("time,score,id")
    .from("scores")
    .where("name = :name")
    .use("time", "100")
    .execute(result);
		
    if(result.isOK()) {
    while(result.next()) {
    cout << result.getInt(1) << " - ";
    cout << result.getInt(2) << " - ";
    cout << result.getString(0) << endl;
    }
    }
  */

} // roxlu 

#endif

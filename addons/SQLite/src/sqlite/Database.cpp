#include <sqlite/Database.h>
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>

int roxlu_database_busy_handler(void* v, int r) {
#if !defined(NDEBUG)
  RX_ERROR("sqlite database seems to be locked.");
#endif
  return 1;
}

namespace roxlu  {



  Database::Database()
    :file("")
    ,opened(false)
  {
  }

  Database::~Database() {
    if(opened) {
      RX_VERBOSE("closing database.");
      sqlite3_close(db);
    }
  }

  //http://icculus.org/~chunky/stuff/sqlite3_example/sqlite3_example_bind.c
  bool Database::open(const string& fileName, bool datapath) {

    file = fileName;
    if(datapath) {
      file = rx_to_data_path(fileName);
    }

    if(SQLITE_OK != sqlite3_open(file.c_str(), &db)) {
      RX_ERROR("Error: %s", sqlite3_errmsg(db));
      sqlite3_close(db);
      return false;
    }
    sqlite3_busy_handler(db, roxlu_database_busy_handler, NULL);
    opened = true;
    return true;
  }

  Query Database::query(const string& sql) {
    Query q(*this, sql);

    if(!opened) {
      RX_WARNING("Warning query(): db not opened");
      return q;
    }

    return q;
  }

  bool Database::beginTransaction() {
    return (SQLITE_OK == sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0));
  }

  bool Database::endTransaction() {
    return (SQLITE_OK == sqlite3_exec(db, "COMMIT", 0, 0, 0));
  }

  int Database::lastInsertID() {
    return sqlite3_last_insert_rowid(db);
  }

  QueryInsert Database::insert(const string& table) {
    QueryInsert insert(*this, table);
    return insert;
  }

  QueryUpdate Database::update(const string& table) {
    QueryUpdate update(*this, table);
    return update;
  }

  QuerySelect Database::select(const string& selectFields) {
    QuerySelect select(*this, selectFields);
    return select;
  }

  QuerySelect Database::select() {
    QuerySelect select(*this);
    return select;	
  }

  QueryDelete Database::remove() {
    QueryDelete remove(*this);
    return remove;
  }

  QueryDelete Database::remove(const string& table) {
    QueryDelete remove(*this, table);
    return remove;
  }


  // THE CALLER MUST FINALIZE() the statement when ready!
  bool Database::prepare(const string& sql, sqlite3_stmt** stmt) {
    if(!opened) {
      RX_WARNING("Warning prepare(): db not opened");
      return false;
    }
    if(SQLITE_OK != sqlite3_prepare_v2(db, sql.c_str(), -1, stmt, 0)) {
      RX_ERROR("DB error: %s with prepare: %s\n", sqlite3_errmsg(db), sql.c_str());
      return false;
    }
    return true;
  }

  // Bind values in given QueryParams
  bool Database::bind(const vector<QueryParam*>& params, sqlite3_stmt** stmt, int queryType) {
    vector<QueryParam*>::const_iterator it = params.begin();
    //printf("bind count: %d\n", sqlite3_bind_parameter_count(*stmt));
    while(it != params.end()) {
      QueryParam* qp = (*it);
      int parameter_index = sqlite3_bind_parameter_index(*stmt, qp->getBindName().c_str());
		
      // sqlite quirk, the index count sometimes start at 1 depending on query
      /*
        Wierd: sometimes I need index +1 sometimes not....
        queryType == QUERY_SELECT
        ||
      */
		
      if(queryType == QUERY_DELETE) {
        parameter_index += 1; 
      }
		
      switch(qp->getType()) {
		
        // TEXT
        // ---------------
        case QueryParam::SQL_PARAM_TEXT: {
          //printf("bind: %s,  %s, %d\n",qp->getBindName().c_str(), qp->getValue().c_str(), parameter_index);
          int result = sqlite3_bind_text(
                                         *stmt
                                         ,parameter_index
                                         ,qp->getValue().c_str()
                                         ,-1
                                         ,SQLITE_TRANSIENT
                                         );
							
          if(result != SQLITE_OK) {
            RX_ERROR("Error: cannot bind: %s(%d) =  %s", qp->getField().c_str(),parameter_index, sqlite3_errmsg(db));
            return false;
          }

          break;
        }
				
        default: {
          break;
        }
      }

      ++it;
    }
    return true;
  }

  void Database::printCompileInfo() {
    sqlite3_stmt* st;
    if (SQLITE_OK == sqlite3_prepare_v2(db, "PRAGMA compile_options", -1, &st, NULL)) {
      while (sqlite3_step(st) == SQLITE_ROW) {
        printf("---- %s\n", sqlite3_column_text(st, 0));
      }
    }
  }

} // roxlu 

#ifndef ROXLU_DATABASE_QUERYH
#define ROXLU_DATABASE_QUERYH

#include "sqlite3.h"

namespace roxlu {

class Database;

class Query {
public:
	Query(Database& db);
	~Query();
	Database& getDB();
	sqlite3* getSQLite();
protected:
	Database& db;
};

} // roxlu
#endif
#include "Query.h"
#include "Database.h"

namespace roxlu {

Query::Query(Database& db)
	:db(db)
{
}

Query& Query::operator=(const Query& other) {
	db = other.db;
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

};
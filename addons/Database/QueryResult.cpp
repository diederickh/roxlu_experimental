#include "QueryResult.h"
#include "Database.h"
#include <sstream>

namespace roxlu {

QueryResult::QueryResult(Database& db) 
	:db(db)
	,is_ok(true)
	,stmt(NULL)
	,row_index(0)
{
}

QueryResult& QueryResult::operator=(const QueryResult& other) {
	if(this == &other) {
		return *this;
	}
	//printf("copy query: %p", other.stmt);
	db = other.db;
	stmt = other.stmt;
	is_ok = other.is_ok;
	row_index = other.row_index;
	last_result = other.last_result;
	return *this;
}

QueryResult::QueryResult(const QueryResult& other) 
	:db(other.db)
{
	*this = other;
}

QueryResult::~QueryResult() {
	// @todo we need to "finalize" the stmt somewhere!!
	
	//printf("remove query.... %p\n", stmt);
//	if(stmt != NULL && SQLITE_OK != sqlite3_finalize(stmt)) {
//		printf("Error: cannot finalit statement.\n");
//	}
}

bool QueryResult::execute(const string& sql, QueryParams& params, int queryType) {
	if(!getDB().prepare(sql, &stmt)) {
		sqlite3_finalize(stmt);
		printf("error: cannot prepare\n");
		return false;
	}
	//printf("db: %p\n", &getDB());
	if(!getDB().bind(params.getParams(), &stmt, queryType)) {
		printf("error: cannot bind..\n");

		//sqlite3_finalize(stmt);
		return false;
	}
	row_index = 0;
	return true;
}

// is this the last entry in the result set.
bool QueryResult::isLast() {
	return last_result != SQLITE_ROW;
}

bool QueryResult::next() {
	last_result = sqlite3_step(stmt);
	return last_result == SQLITE_ROW;
}

string QueryResult::getString(int index) {
	if(last_result != SQLITE_ROW) {
		return "";
	}
	std::stringstream ss;
	ss << sqlite3_column_text(stmt, index);
	return ss.str();
}

int64_t QueryResult::getInt(int index) {
	if(last_result != SQLITE_ROW) {
		return 0;
	}
	return sqlite3_column_int(stmt, index);
}

float QueryResult::getFloat(int index) {
	if(last_result != SQLITE_ROW) {
		return 0;
	}
	return sqlite3_column_double(stmt, index);
}


} // roxlu
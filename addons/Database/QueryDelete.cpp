#include "QueryDelete.h"
#include "Database.h"
#include <stdlib.h>

namespace roxlu {

QueryDelete::QueryDelete(Database& db) 
	:Query(db)
	,from_table("")
	,where_clause("")
{
}

QueryDelete::QueryDelete(Database& db, const string& fromTable)
	:Query(db)
	,from_table(fromTable)
	,where_clause("")
{
}

QueryDelete::QueryDelete(const QueryDelete& other) 
	:Query(other.db)
{
	*this = other;
}

QueryDelete& QueryDelete::operator=(const QueryDelete& other) {
	if(this == &other) {
		return *this;
	}
	
	where_clause = other.where_clause;
	field_values = other.field_values;
	from_table = other.from_table;
	return *this;
}

QueryDelete::~QueryDelete() {
}

QueryDelete& QueryDelete::from(const string& fromTable) {
	from_table = fromTable;
	return *this;
}

QueryDelete& QueryDelete::where(const string& whereClause) {
	where_clause = whereClause;
	return *this;
}

string QueryDelete::toString() {
	string sql;
	if(!from_table.length()) {
		printf("Error: cannot delete, no from table set.\n");
		exit(0);
	}
	sql.append("delete from ");
	sql.append(from_table);
	
	if(where_clause.length()) {
		sql.append(" where ");
		sql.append(where_clause);
	}
	
	return sql;
}

bool QueryDelete::execute() {
	string sql = toString();
	if(!sql.length()) {
		return false;
	}
	
	sqlite3_stmt* stmt;
	if(!getDB().prepare(sql, &stmt)) {
		sqlite3_finalize(stmt);
		return false;
	}
	
	if(!getDB().bind(field_values.getParams(), &stmt, Database::QUERY_DELETE)) {
		sqlite3_finalize(stmt);
		return false;
	}
		
	if(sqlite3_step(stmt) != SQLITE_DONE) {
		printf("error: %s\n", sqlite3_errmsg(getSQLite()));
		sqlite3_finalize(stmt);
		return false;
	}
	sqlite3_finalize(stmt);
	return true;

}

} // roxlu
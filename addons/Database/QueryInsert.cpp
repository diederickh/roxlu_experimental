#include "QueryInsert.h"
#include "Database.h"

namespace roxlu {

QueryInsert::QueryInsert(Database& db)
	:Query(db)
{
}

QueryInsert::QueryInsert(Database& db, const string& table) 
	:Query(db)
	,table(table)
{
}

QueryInsert::QueryInsert(const QueryInsert& other)
	:Query(other.db)
{
	*this = other;
}


QueryInsert& QueryInsert::operator=(const QueryInsert& other) {
	if(this == &other) {
		return *this;
	}	
	
	or_clause = other.or_clause;
	table = other.table;
	field_values = other.field_values;
	return *this;
}

QueryInsert::~QueryInsert() {
}

// Creates the sql with params.
string QueryInsert::toString() {
	string sql = "";
	string fields;
	string values;
	
	if(!field_values.getFieldList(fields)) {
		return sql;
	}
	
	if(!field_values.getValueList(values)) {
		return sql;
	}
	
	sql.append("insert ");
	
	if(or_clause.length()) {
		sql.append(or_clause);
	}
	
	sql.append(" into ");
	sql.append(table);
	sql.append("(");
	sql.append(fields);
	sql.append(") values (");
	sql.append(values);
	sql.append(")");
	
	//printf("> %s\n", sql.c_str());
	return sql;
}

// Execute the query
bool QueryInsert::execute() {
	string sql = toString();
	if(!sql.length()) {
		return false;
	}

	sqlite3_stmt* stmt;
	if(!getDB().prepare(sql, &stmt)) {
		sqlite3_finalize(stmt);
		return false;
	}
	
	if(!getDB().bind(field_values.getParams(), &stmt, Database::QUERY_INSERT)) {
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
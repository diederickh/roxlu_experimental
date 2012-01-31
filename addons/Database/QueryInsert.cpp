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
	
	sql.append("insert into ");
	sql.append(table);
	sql.append("(");
	sql.append(fields);
	sql.append(") values (");
	sql.append(values);
	sql.append(")");
	
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
#include "Database.h"
#include "QuerySelect.h"

namespace roxlu {

QuerySelect::QuerySelect(Database& db)	
	:Query(db)
	,select_fields("*")
	,where_clause("")
{
}

QuerySelect::QuerySelect(Database& db, const string& selectFields) 
	:Query(db)
	,select_fields(selectFields)
{
	
}

QuerySelect::~QuerySelect() {
}

QuerySelect& QuerySelect::from(const string& table) {
	from_table = table;
	return *this;
}

QuerySelect& QuerySelect::where(const string& whereClause) {
	where_clause = whereClause;
	return *this;
}

string QuerySelect::toString() {
	string sql = "";
	sql.append("select ");
	sql.append(select_fields);
	sql.append(" from ");
	sql.append(from_table);
	
	if(where_clause.length()) {
		sql.append(" where ");
		sql.append(where_clause);
		sql.append(" ");
	}
	return sql;
	
}

bool QuerySelect::execute(QueryResult& result) {
	string sql = toString();
	if(!sql.length()) {
		return false;
	}
	printf("query: %s\n", sql.c_str());
	return result.execute(sql, field_values, Database::QUERY_SELECT);
}

} // roxlu
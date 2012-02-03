#ifndef ROXLU_DATABASE_QUERY_DELETEH
#define ROXLU_DATABASE_QUERY_DELETEH

#include "Query.h"
#include <string>
#include "QueryResult.h"

using std::string;

namespace roxlu {

class Database;

class QueryDelete : public Query {
public:
	QueryDelete(Database& db);
	QueryDelete(Database& db, const string& fromTable);
	QueryDelete(const QueryDelete& other);
	QueryDelete& operator=(const QueryDelete& other);
	~QueryDelete();
	
	QueryDelete& from(const string& fromTable);
	QueryDelete& where(const string& whereClause);
	bool execute();
	
	string toString();
		
				
	template<typename T>
	QueryDelete& use(const string& fieldName, const T& fieldValue) {
		field_values.use(fieldName, fieldValue);
		return *this;
	}
	
	

private:
	string where_clause;
	QueryParams field_values;
	string from_table;
};

} // roxlu

#endif
#ifndef ROXLU_DATABASE_QUERY_SELECTH
#define ROXLU_DATABASE_QUERY_SELECTH

#include "Query.h"
#include <string>
#include "QueryResult.h"

using std::string;

namespace roxlu {

class Database;

class QuerySelect : public Query {
public:
	QuerySelect(Database& db);
	QuerySelect(Database& db, const string& selectFields);
	~QuerySelect();
	
	QuerySelect& from(const string& fromTable);
	bool execute(QueryResult& result);
	string toString();
	
	template<typename T>
	QuerySelect& use(const string& fieldName, const T& fieldValue) {
		field_values.use(fieldName, fieldValue);
		return *this;
	}

	QuerySelect& where(const string& whereClause);
	
	/*
	template<typename T>
	QuerySelect& where(const string& fieldName, const T& fieldValue) {
		where_values.use(fieldName, fieldValue);
		return *this;
	}
	*/
	
	
private:
	string select_fields;
	string from_table;
	string where_clause;
//	QueryParams where_values;
	QueryParams field_values;
	
};

} // roxlu
#endif

/*
#ifndef ROXLU_DATABASE_QUERY_INSERTH
#define ROXLU_DATABASE_QUERY_INSERTH

#include "Query.h"
#include "QueryParams.h"

namespace roxlu {

class Database;
class QueryInsert : public Query {
public: 
	QueryInsert(Database& db);
	QueryInsert(Database& db, const string& table);
	~QueryInsert();	
	
	QueryInsert& setTable(const string& tableName);
	
	template<typename T>
	QueryInsert& use(const string& fieldName, const T& fieldValue) {
		field_values.use(fieldName, fieldValue);
		return *this;
	}
	string toString();
	
	virtual bool execute();
protected:
	string table;
	QueryParams field_values;
};

} // roxlu
#endif
*/
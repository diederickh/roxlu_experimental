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
	
	bool execute();
protected:
	string table;
	QueryParams field_values;
};

} // roxlu
#endif
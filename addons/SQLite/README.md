# SQLite 

This addons provides a couple of simple wrappers aroudn the `sqlite3` library.


### Insert

````c++
  #include <sqlite/Database.h>
  {
    QueryResult qr(db);
    db.query("CREATE TABLE IF NOT EXISTS videos("
             "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
             "    filename TEXT, "
             "    state INTEGER DEFAULT 0)").execute(qr);
    qr.finish();
  }

  // insert
  if(!db.insert("state").use("refresh_token", rtoken).execute()) {
    RX_ERROR("Cannot update/replace the refresh token");
  }

````

### Insert or update
````c++
  #include <sqlite/Database.h>
  {
    QueryResult qr(db);
    db.query("CREATE TABLE IF NOT EXISTS state("
             "   name TEXT UNIQUE, "
             "   value TEXT)").execute(qr);
    qr.finish();
   
  }

  // insert OR REPLACE! FIELD MUST BE UNIQUE
  if(!db.insert("state").use("name","refresh_token").use("value", 122).orReplace().execute()) {
    RX_ERROR("Cannot update/replace the refresh token");
  }

````

### Update
````c++
  #include <sqlite/Database.h>
  {
    QueryResult qr(db);
    db.query("CREATE TABLE IF NOT EXISTS state("
             "   name TEXT UNIQUE, "
             "   value TEXT)").execute(qr);
    qr.finish();
  }
  
  db.update("state").use("value", 1235).where("name", "refresh_token").execute();
````


## Using SQLite to keep application state

Sometime it's handy to use a database to keep track of you application state. A common
solution is to create a state table into which you store key-value pairs:

````c++

    QueryResult qr(db);
    db.query("CREATE TABLE IF NOT EXISTS state("
             "   name TEXT UNIQUE, "
             "   value TEXT)").execute(qr);
    qr.finish();

````

Then if you want to insert or update a value, you use the `orReplace()` feature of
sqlite3. Though you must make sure that *you've set the `name` field to UNIQUE*.
Then inserting or updating a value can be done like:

````c++
void YouTubeModel::setRefreshToken(std::string rtoken) {
  if(!db.insert("state").use("name", "refresh_token").value("value", rtoken).orReplace().execute()) {
    RX_ERROR("Cannot update/replace the refresh token");
  }
}

void YouTubeModel::setAccessToken(std::string atoken, uint64_t timeout) {
  if(!db.insert("state").use("name", "access_token").use("value", atoken).orReplace().execute()) {
    RX_ERROR("Cannot update/replace the access token");
  }

  if(!db.insert("state").use("name", "token_timeout").use("value", timeout).orReplace().execute()) {
    RX_ERROR("Cannot update/replace the token timeout");
  }
}

````

A simple way to retrieve values:
````c++
inline std::string YouTubeModel::getRefreshToken() {
  QueryResult qr(db);
  db.select("value").from("state").where("name = 'refresh_token'").execute(qr);

  qr.next();
    std::string rtoken = qr.getString(0);
  qr.finish();

  return rtoken;
}

inline std::string YouTubeModel::getAccessToken() {
  QueryResult qr(db);
  db.select("value").from("state").where("name = 'access_token'").execute(qr);

  qr.next();
    std::string atoken = qr.getString(0);
  qr.finish();

  return atoken;
}

````
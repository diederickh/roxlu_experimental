Database
==================================

Tutorial
------------------------

The Database addon is a simple wrapper around SQLite.  This tutorial will 
guide you through settings up a simple database and doing some queries 
on it. Make sure to include the Database.h header file.
Then you need to create a database object.


.. code-block:: c++

	roxlu::Database db
	
	
In your setup code you open the database by calling db.open("filename"):

.. code-block:: c++

	db.open("my_database.db");
	db.query("CREATE TABLE IF NOT EXISTS scores (" \
			" id INTEGER PRIMARY KEY AUTOINCREMENT" \
			", score INTEGER" \
			", name TEXT "\
		");");
		
		
		
	

Examples
--------

.. code-block:: c++

	bool result = db.query(
			   "CREATE TABLE IF NOT EXISTS tweets( "					\
			   " t_id			INTEGER PRIMARY KEY AUTOINCREMENT"	\
			   ",t_user_id		VARCHAR(50)"							\
			   ",t_text			INTEGER"								\
			   ",t_screen_name	VARCHAR(20)"							\
			   ",t_timestamp	TIMESTAMP"								\	
			   ",t_longitude	REAL"									\
			   ",t_latitude	REAL"									\
			   ");"
   );

	QueryResult result(db);
	db.select("time,score,id")
		.from("scores")
		.where("name = :name")
		.use("time", "100")
		.execute(result);

	// Some date and time examples;
	// select datetime('now','localtime','+3.5 seconds','+10 minutes');
	QueryResult result(db);
	db.select("t_text, t_user_id, t_timestamp,  datetime('now', '-1.5 hours') as test, tt_tag")
		.from("tweets")
		.join("tweet_tags on tt_id = t_id")
		.where("tt_tag = :tt_tag ")
		.use("tt_tag", word)
		.execute(result);
	
	// create index
	result = db.query("CREATE INDEX tweet_timestamp ON tweets(t_timestamp)");
	
	// combined primary key
		result = db.query(
			  "CREATE TABLE IF NOT EXISTS tweet_tags( "				\
			  " tt_tagid			INTEGER "							\
			  ",tt_tweetid			INTEGER "						\
			  ",PRIMARY KEY(tt_tagid, tt_tweetid) "
			  ");"
	  );


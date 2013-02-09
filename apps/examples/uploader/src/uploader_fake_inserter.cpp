#include <iostream>
#include <stdio.h>
#include <string>
#include <roxlu/Roxlu.h>
#include <kurl/Kurl.h>
#include <sqlite/Database.h>


/**
 * UPLOAD TEST APPLICATION
 * -----------------------
* Created to simulate new inserts into the upload que database.
 */

INI uploader_ini;
Database uploader_db;

int main(int argc, char** argv) {
  if(argc != 2) {
    printf("Usage: ./upload_fake_inserter /path/to/uploader.ini\n");
    return EXIT_FAILURE;
  }

  std::string ini_path = argv[1];
  if(!uploader_ini.load(ini_path)) {
    printf("ERROR: cannot open ini file\n");
    return EXIT_FAILURE;
  }

  if(!uploader_ini.load(ini_path)) {
    printf("ERROR: cannot start uplaoder, uploader.ini not found in: %s\n", ini_path.c_str());
    return EXIT_FAILURE;
  }

  std::string database_path = uploader_ini.getString("database_path", "none");
  if(database_path == "none") {
    printf("ERROR: cannot get database path.\n");
    return EXIT_FAILURE;
  }

  std::string fake_insert_query = uploader_ini.getString("fake_insert_query", "none");
  if(fake_insert_query == "none") {
    printf("ERROR: cannot get fake_insert_query from settings.\n");
    return EXIT_FAILURE;
  }

  int timeout = uploader_ini.getInt("fake_insert_timeout_millis", 1000);

  printf("\n\n");
  printf("fake upload inserter v.0.0.1\n");
  printf("---------------------------------------\n");

  printf("ini_path = %s\n", ini_path.c_str());
  printf("database_path = %s\n", database_path.c_str());
  printf("fake_insert_query: %s\n", fake_insert_query.c_str());
  printf("timeout: %d\n", timeout);
  printf("---------------------------------------\n\n");
  
  if(!uploader_db.open(database_path)) {
    printf("ERROR: cannot open database. %s\n", database_path.c_str());
    return EXIT_FAILURE;
  }

  while(true) {
    printf("%s\n", fake_insert_query.c_str());
    QueryResult qr(uploader_db);
    uploader_db.query(fake_insert_query).execute(qr);
    qr.finish();

    rx_sleep_millis(timeout);
  }
  return 0;
}

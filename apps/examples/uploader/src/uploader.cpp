/**
 * GENERIC UPLOADER / FORM POSTER
 * -------------------------------
 *
 * This util queries a SQLite database and performs a 
 * HTTP POST with information from this table. 
 *
 * To make sure every file gets uploaded, we use a a couple
 * of states: 
 * -----------------------
 * 0 - file has been added 
 * 1 - file is currently being uploaded
 * 2 - file is uploaded.
 * 3 - file is uploaded and removed from disk.
 * -----------------------
 *
 */

extern "C" {
#  include <uv.h>
}

#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <string>
#include <roxlu/Roxlu.h>
#include <curl/Kurl.h>
#include <sqlite/Database.h>

/* settings, uploader, db */
INI uploader_ini;
Kurl uploader_kurl;
Database uploader_db;
uv_loop_t* loop;
uv_tty_t tty;
rx_int64 delete_timeout;                                /* we check every X-millis for files that we need to remove; we don't need to perform this operation all the time */
rx_int64 delete_delay;                                  /* the time in millis that we check files that need to be deleted */
rx_int64 delete_after_days;                             /* set int the settings, delete files older then X-days */
rx_int64 delete_after_hours;                            /* added to the delete_after_days, delete files older then X-hours */
bool must_delete;                                       /* set in the settings, if you want to delete files, set it to 1 in the settings */
bool test_mode;                                         /* when you start this app with --test [filename], this is set to true and we will test a file upload */
bool test_in_progress;                                  /* set to true when we're actually uploading */

rx_int64 total_kbytes_to_upload;                        /* total number of kilo bytes in to upload */
rx_int64 total_kbytes_uploaded;                         /* total bytes uploaded */

/* settings */
std::string ini_path;                                    /* path where we can find the ini settings, must be passed as argument to this application */
std::string upload_url;                                  /* the url we post the file + field to */
std::string src_files_path;                              /* in the database table you store the filename that we must upload, the files are located in this directory */
std::string database_path;                               /* path to the sqlite database we perform queries on */

/* queries - db */
std::string id_field;                                    /* the primary key of the table, used to update the 'uploaded' field. */
std::string file_field;                                  /* this is the field in the table that is used to fetch the file we need to upload */
std::string get_total_entries_to_upload_query;           /* query that returns how many new entries there are in the database which havent been uploaded, files with a upload state 0 need to be uploaded */
std::string get_entries_to_upload_query;                 /* query that returns all info about the entries that need to be uploaded (state = 0) */
std::string set_upload_state_query;                      /* update query that is used to change the state. we use printf(), the first value is the state the second one the value of the 'id_field', we assume ids are number */
std::string reset_upload_state_query;                    /* when the application restarts after a crash we reset the states from 1 to 0 for the 'upload' state field */
std::string delete_files_query;                          /* query which returns the entries for which we can delete the associated files, the entry will stay in the db */

int total_uploading = 0;
int total_uploaded = 0;

char sql_buf[1024];

struct upload_entry {
  int id;
  std::string filepath;
  std::map<std::string, std::string> fields;
};

enum color {
  BLACK = 0,
  RED = 1,
  GREEN = 2,
  YELLOW = 3,
  BLUE = 4,
  MAGENTA = 5,
  CYAN = 6,
  WHITE = 7
};

int spinner_dx = 0;

void log(std::string msg, color fg = WHITE);
void print_spinner();
void print_settings();
int load_settings();
void delete_files();
int cb_progress(void* user, double dltotal, double dlnow, double ultotal, double ulnow);   /* used to show info on the upload progress */


void cb_post_complete(KurlConnection* c, void *user) {

  upload_entry* ue = static_cast<upload_entry*>(user);
  memset(sql_buf, 0, sizeof(sql_buf));
  sprintf(sql_buf, set_upload_state_query.c_str(), 2, ue->id);

  QueryResult update_state(uploader_db);
  uploader_db.query(sql_buf).execute(update_state);
  update_state.finish();

  total_uploaded++;

  delete ue;
  ue = NULL;
}

int main(int argc, char** argv) {
  test_mode = false;
  test_in_progress = false;
  total_kbytes_to_upload = 0;
  total_kbytes_uploaded = 0;
  delete_delay = 4 * 1000;
  delete_timeout = rx_millis() + delete_delay;
  loop = uv_default_loop();

  uv_tty_init(loop, &tty, 1, 0);
  uv_tty_set_mode(&tty, 0);
  
  if(uv_guess_handle(1) != UV_TTY) {
    printf("Not a compatibly TTY terminal; expect some wierd output\n");
  }

  if(argc != 2 && argc != 4) {
    log("ERROR: usage: ./uploader settings.ini\n", RED);
    log("ERROR: usage: ./uploader settings.ini --test somefile.png\n", RED);
    return EXIT_FAILURE;
  }
  
  if(argc > 2 && strcmp(argv[2], "--test") == 0) {
    if(argc != 4) {
      log("ERROR: when using the --test flag, make sure to pass a filename to test with\n", RED);
      return EXIT_FAILURE;
    }
    test_mode = true;
  }

  log("uploader v.0.0.1\n", RED);

  ini_path = argv[1];
  if(!uploader_ini.load(ini_path)) {
    ini_path = rx_to_exe_path(argv[1]);
    if(!uploader_ini.load(ini_path)) {
      log("ERROR: cannot open ini file", RED);
    }
    return EXIT_FAILURE;
  }

  if(load_settings() == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  print_settings();

  if(!uploader_db.open(database_path)) {
    log("ERROR: cannot open database.", RED); log(database_path, RED);  log("\n", BLACK);
    return EXIT_FAILURE;
  }

  QueryResult state_result(uploader_db);
  bool r = uploader_db.query(reset_upload_state_query).execute(state_result);
  if(!r) {
    log("ERROR: cannot reset files which were being uploaded while application crashed/exited\n", RED);
  }
  state_result.finish();

  uploader_kurl.setProgressCallback(cb_progress, NULL);

  log("> reset previously failed upload entries\n", MAGENTA);

  while(true) {
    // TEST MODE
    if(test_mode) {
      if(!test_in_progress) {
        upload_entry* e = new upload_entry();
        e->id = 0;
        e->filepath = rx_to_exe_path(argv[3]);
        log("TEST: uploading: ", YELLOW);  log(e->filepath.c_str(), RED); log("\n", BLACK);

        Form f;
        f.setURL(upload_url);
        f.addFile("file", rx_to_exe_path(argv[3]));
        if(!uploader_kurl.post(f, cb_post_complete, e)) {
          log("ERROR: cannot perform test upload\n", RED);
          return EXIT_FAILURE;
        }
        test_in_progress = true;
      }
      else {
        uploader_kurl.update();
      }
      continue;
    }

    // PRODUCTION
    QueryResult rows_result(uploader_db);
    r =  uploader_db.query(get_total_entries_to_upload_query).execute(rows_result);
    if(!r) {
      log("WARNING: the query to get the total number of new videos did not work.\n", RED);
      rx_sleep_millis(500);
      continue;
    }

    rows_result.next();
    if(rows_result.getInt(0) == 0) {
      print_spinner();
      rows_result.free();

      for(int i = 0; i < 50; ++i) {
         uploader_kurl.update();
      }

      delete_files();

      rx_sleep_millis(100);
      continue;
    }
    
    QueryResult fields_result(uploader_db);
    r = uploader_db.query(get_entries_to_upload_query).execute(fields_result);
    if(!r) {
      log("WARNING: cannot execute get_entries_to_upload_query\n", RED);
      rx_sleep_millis(100);
      continue;
    }
    
    std::vector<std::string> fields;
    fields = fields_result.getFieldNames();

    while(fields_result.next()) {
      Form f;
      f.setURL(upload_url);

      /* store all field values + add them to a form */
      upload_entry* e = new upload_entry();
      for(int i = 0; i < fields.size(); ++i) { 
        std::string field_name = fields[i];
        e->fields[field_name] = fields_result.getString(i);
        
        if(field_name == id_field) {
          e->id = fields_result.getInt(i);
          f.addInput(field_name, e->fields[field_name]);
        }
        else if(field_name == file_field) {
          e->filepath = src_files_path + e->fields[field_name];
          f.addFile("file", e->filepath);
        }
        else {
          f.addInput(field_name, e->fields[field_name]);
        }
      }

      /* update the 'upload' status to indicate we're uploading */
      {
        log("ID: ", YELLOW); log(e->fields[id_field]); log(" uploading: ", GREEN);  log(e->filepath, CYAN); log("\n", BLACK);

        memset(sql_buf, 0, sizeof(sql_buf));
        sprintf(sql_buf, set_upload_state_query.c_str(), 1, e->id);

        QueryResult update_state(uploader_db);
        uploader_db.query(sql_buf).execute(update_state);
        update_state.finish();
      }

      if(!uploader_kurl.post(f, cb_post_complete, e)) {
        printf("ERROR: cannot post form.\n");
      }      
      else {
        total_uploading++;
      }
    }
    uploader_kurl.update();

  }

  return EXIT_SUCCESS;
}

/* check if there are files that we need to remove */
void delete_files() {
  rx_int64 now = rx_millis();
  time_t epoch = rx_time();
  
  //  rx_int64 delete_days = 0;
  //  rx_int64 delete_hours = 5;
  rx_int64 delete_seconds = (delete_after_days * 24 * 60 * 60) + (delete_after_hours * 60 * 60);
  rx_int64 delete_before = epoch - delete_seconds;

  if(now > delete_timeout) {
    delete_timeout = now + delete_delay;
    QueryResult delete_result(uploader_db);
    uploader_db.query(delete_files_query).execute(delete_result);

    while(delete_result.next()) {
      std::string filepath = src_files_path + delete_result.getString(1);

      rx_int64 mtime = File::getTimeModified(filepath);
      if(mtime < delete_before) {
        File::remove(filepath.c_str());
      }

      /* change the state to deleted */
      rx_int64 id = delete_result.getInt(0);
      memset(sql_buf, 0, sizeof(sql_buf));
      sprintf(sql_buf, set_upload_state_query.c_str(), 3, id);

      QueryResult update_state(uploader_db);
      uploader_db.query(sql_buf).execute(update_state);
      update_state.finish();

    }
  }
}

int cb_progress(void* user, double dltotal, double dlnow, double ultotal, double ulnow) {
  total_kbytes_to_upload = ultotal / 1024;
  total_kbytes_uploaded = ulnow / 1024;
  return 0;
}


int load_settings() {
  upload_url = uploader_ini.getString("upload_url", "none");
  if(upload_url == "none") {
    log("ERROR: cannot find the upload_url in uploader.ini", RED);
    return EXIT_FAILURE;
  }

  src_files_path = uploader_ini.getString("src_files_path", "none");
  if(src_files_path == "none") {
    log("ERROR: cannot find the src_files_path in uploader.ini", RED);
    return EXIT_FAILURE;
  }
  
  database_path = uploader_ini.getString("database_path", "none");
  if(database_path == "none") {
    log("ERROR: cannot get database path.\n", RED);
    return EXIT_FAILURE;
  }
  
  get_total_entries_to_upload_query = uploader_ini.getString("get_total_entries_to_upload_query", "none");
  if(get_total_entries_to_upload_query == "none") {
    log("ERROR: cannot get ini field: get_total_entries_to_upload_query  .\n", RED);
    return EXIT_FAILURE;
  }

  get_entries_to_upload_query = uploader_ini.getString("get_entries_to_upload_query", "none");
  if(get_entries_to_upload_query == "none") {
    log("ERROR: cannot get ini field: get_entries_to_upload_query  .\n", RED);
    return EXIT_FAILURE;
  }

  set_upload_state_query = uploader_ini.getString("set_upload_state_query", "none");
  if(set_upload_state_query == "none") {
    log("ERROR: cannot get ini field: set_upload_state_query  .\n", RED);
    return EXIT_FAILURE;
  }

  reset_upload_state_query = uploader_ini.getString("reset_upload_state_query", "none");
  if(reset_upload_state_query == "none") {
    log("ERROR: cannot get ini field: reset_upload_state_query.\n", RED);
    return EXIT_FAILURE;
  }

  file_field = uploader_ini.getString("file_field", "none");
  if(file_field == "none") {
    log("ERROR: cannot get ini field: file_field\n", RED);
    return EXIT_FAILURE;
  }

  id_field = uploader_ini.getString("id_field", "none");
  if(id_field == "none") {
    log("ERROR: cannot get ini field: id_field\n", RED);
    return EXIT_FAILURE;
  }

  delete_files_query = uploader_ini.getString("delete_files_query", "none");
  if(delete_files_query == "none") {
    log("ERROR: cannot get ini field: delete_files_query\n", RED);
    return EXIT_FAILURE;
  }

  delete_after_days = uploader_ini.getInt("delete_after_days", -1);
  if(delete_after_days == -1) {
    log("ERROR: cannot get ini field: delete_after_days (or dit you use -1?)");
    return EXIT_FAILURE;
  }

  delete_after_hours = uploader_ini.getInt("delete_after_hours", -1);
  if(delete_after_hours == -1) {
    log("ERROR: cannot get ini field: delete_after_hours (or dit you use -1?)");
    return EXIT_FAILURE;
  }
  
  must_delete = uploader_ini.getBool("must_delete", false);
  

  return EXIT_SUCCESS;
}

void print_settings() {
  char ddays[128];
  char dhours[128];
  char dmust[128];
  sprintf(ddays, "%d", int(delete_after_days));
  sprintf(dhours, "%d", int(delete_after_hours));
  sprintf(dmust, "%c", must_delete ? 'Y' : 'N');

  log("-----------------------------------------------------------------------------------\n", YELLOW);
  log("upload url = ", GREEN); log(upload_url, CYAN); log("\n");
  log("ini path = ", GREEN); log(ini_path, CYAN); log("\n");
  log("source files path = ", GREEN); log(src_files_path, CYAN); log("\n");
  log("database path = ", GREEN); log(database_path, CYAN); log("\n");
  log("-----------------------------------------------------------------------------------\n", YELLOW);
  log("delete_after_days = ", GREEN); log(ddays, CYAN); log("\n");
  log("delete_after_hours = ", GREEN); log(dhours, CYAN); log("\n");
  log("must_delete = ", GREEN); log(dmust, CYAN); log("\n");
  log("-----------------------------------------------------------------------------------\n", YELLOW);
  log("file_field = ", GREEN); log(file_field, CYAN); log("\n");
  log("id_field = ", GREEN); log(id_field, CYAN); log("\n");
  log("get_total_entries_to_upload_query = ", GREEN); log(get_total_entries_to_upload_query, CYAN); log("\n");
  log("get_entries_to_upload_query = ", GREEN); log(get_entries_to_upload_query, CYAN); log("\n");
  log("set_upload_state_query = ", GREEN); log(set_upload_state_query, CYAN); log("\n");
  log("reset_upload_state_query = ", GREEN); log(reset_upload_state_query, CYAN); log("\n");
  log("-----------------------------------------------------------------------------------\n", YELLOW);
}

void log(std::string msg, color fg) {
  uv_buf_t buf;
 
  char str[1024];
  sprintf(str, "\033[1;%d;40m%s\033[37;40m", (30 + fg), msg.c_str());
  buf.base = str;
  buf.len = strlen(str);

  uv_write_t req;
  uv_write(&req, (uv_stream_t*)&tty, &buf, 1, NULL);
  uv_run(loop, UV_RUN_DEFAULT);
}

void print_spinner() {

  int num_dashes = 10;
  ++spinner_dx %= num_dashes;
  std::stringstream ss_spinner;

  ss_spinner << "[";
  for(int i = 0; i < num_dashes; ++i) {
    if(i == spinner_dx) {
      ss_spinner << "##";
    }
    else {
      ss_spinner << '-';
    }
  }
  ss_spinner << "]";
  
  char progress[100];
  sprintf(progress, "%d/%d", total_uploaded, total_uploading);

  char bytes_progress[512];
  sprintf(bytes_progress, "%lld / %lld", total_kbytes_uploaded, total_kbytes_to_upload);

  char str[512];
  const char* info_msg = "> waiting for new entries ";

  sprintf(str, "\033[1;35;40m%s\033[1;37;40m%s \033[36;40m(\033[1;33;40m%s\033[36;40m) \033[36;40m(\033[1;33;40m%s\033[36;40m)\033[%dD",
          info_msg,
          ss_spinner.str().c_str(),
          progress,
          bytes_progress,
          (int)(strlen(info_msg) + int(ss_spinner.str().size()) + strlen(progress)+3 + strlen(bytes_progress)+3)
          );
  
  uv_buf_t buf;
  buf.base = str;
  buf.len = strlen(str);
  uv_write_t req;
  uv_write(&req, (uv_stream_t*)&tty, &buf, 1, NULL);
  uv_run(loop, UV_RUN_DEFAULT);

}

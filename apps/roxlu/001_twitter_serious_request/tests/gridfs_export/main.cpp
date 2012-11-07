#include <iostream>
#include <fstream>

extern "C" {
#include <mongo.h>
#include <gridfs.h>
}

#include <unistd.h> // getcwd
#include <libgen.h> // dirname
#include <fcntl.h>
 
#ifdef __linux__
#include <stdio.h>
#endif


// Start this app with ./compile_and_run.sh
// This exports all images stored in the tweet database
int main() {
  printf("GridFS export files\n");

  // get path to test images.
  char dir[1024];
  if((sprintf(dir, "%s/bin/images/", dirname(getcwd(dir, sizeof(dir))))) == NULL) {
    printf("ERROR: cannot find images dir: %s.\n", dir);
    return 1;
  }
  printf(">> %s\n", dir);

  mongo con[1];
  gridfs gfs[1];
  gridfile gfile[1];
  
  if(mongo_connect(con, "127.0.0.1", 27017)) {
    printf("ERROR: Cannot connect to mongo. Did you start it?\n");
    return 1;
  }

  // init gfs
  int r = gridfs_init(con, "tweet", "images", gfs);
  if(r != MONGO_OK) {
    printf("ERROR: Cannot gridfs_init()\n");
    return 1;
  }

  // Get images
  mongo_cursor cursor[1];
  mongo_cursor_init(cursor, con, "tweet.images.files");
  while(mongo_cursor_next(cursor) == MONGO_OK) {
    bson_iterator it[1];
    if(bson_find(it, mongo_cursor_bson(cursor), "filename")) {
      printf("filename: %s\n", bson_iterator_string(it));
      r = gridfs_find_filename(gfs, bson_iterator_string(it), gfile);
      if(r != MONGO_OK) {
        printf("ERROR: Cannot find file: %s\n", bson_iterator_string(it));
        continue;
      }
      
      if(bson_find(it, mongo_cursor_bson(cursor), "md5")) {
        const char* md5 = bson_iterator_string(it);
        if(bson_find(it, mongo_cursor_bson(cursor), "contentType")) {
          const char* mime = bson_iterator_string(it);
          std::string ext;
          if(strcmp(mime, "image/jpeg") == 0) {
            ext = ".jpg";
          }
          else if(strcmp(mime, "image/png") == 0) {
            ext = ".png";
          }
          std::string fname(md5);
          fname = "images/" +fname +ext;
          FILE* stream = fopen(fname.c_str(), "w+");
          gridfile_write_file(gfile, stream);
          fclose(stream);
        }
      }
    }
  }
  mongo_cursor_destroy(cursor);
  
  return 0;
}

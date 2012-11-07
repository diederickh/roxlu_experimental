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
int main() {
  printf("GridFS test\n");

  // get path to test images.
  char dir[1024];
  if((sprintf(dir, "%s/bin/test_images/", dirname(getcwd(dir, sizeof(dir))))) == NULL) {
    printf("ERROR: cannot find test images dir.\n");
    return 1;
  }
  printf(">> %s\n", dir);

  // Store the test images.
  mongo con[1];
  gridfs gfs[1];
  gridfile gfile[1];
  
  if(mongo_connect(con, "127.0.0.1", 27017)) {
    printf("ERROR: Cannot connect to mongo. Did you start it?\n");
    return 1;
  }

  // initialize gridfs with collection "tweet" and fs prefix "images".
  // this will create two database, images.files (metadata) and 
  // images.chunks (the actual data).
  // 
  // When you connect to mongo with the mongo shell, you can get a list of 
  // all images by using
  // 
  // > use tweet (the database)
  // > db.images.files.find() 
 int r = gridfs_init(con, "tweet", "images", gfs);
  if(r != MONGO_OK) {
    printf("ERROR: cannot initialize gridfs.\n");
    return 1;
  }

  // Create a meta data object
  bson o[1];
  bson_init(o);
  bson_append_string(o, "tweet_id_str", "SOME_ID_HERE");
  /*
  r = gridfile_init(gfs, o, gfile);
  if(r != MONGO_OK) {
    printf("ERROR: Canot set meta data of gridfile\n");
  }
  */
  //  bson_destroy(o);

  // lets insert some data
  int num_images = 4;
  char buf[1024 * 100];
  for(int i = 0; i < num_images; ++i) { 
    char filename[1025];
    sprintf(filename, "%s%d.png", dir, i);

    // open the file.
    int fp = open(filename, O_RDONLY);
    if(!fp) {
      printf("ERROR: Cannot open file: %s\n", filename);
      continue;
    }

    // init a gridfile
    gridfile_writer_init(gfile, gfs, filename, "image/png");
    gfile->meta = o;
    size_t bytes_read = 0;
    while((bytes_read = read(fp, buf, sizeof(buf))) > 0) {
      gridfile_write_buffer(gfile, buf, bytes_read);
      printf("read: %zu\n", bytes_read);
    }
    gridfile_writer_done(gfile);

    close(fp);
  }
  
  return 0;
}

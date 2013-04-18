#ifndef ROXLU_COMMANDLINE_OPTIONS_H
#define ROXLU_COMMANDLINE_OPTIONS_H

extern "C" {
#  include <argtable2.h>
}

#include <string>
#include <vector>

#define CMO_ERR_NO_OPTIONS "No options added."
#define CMO_ERR_ARG_ERR "Couldn't create the argtable2 object."
#define CMO_ERR_NULLCHECK "The arg_nullcheck() failed. Did you add any options?"
#define CMO_ERR_UNKNOWN_TYPE "Unknown type! not handled."
#define CMO_ERR_NO_DOUBLE "No double type, just returning 0.0."
#define CMO_ERR_NO_INTEGER "No integer type, just returning 0."
#define CMO_ERR_NO_STRING "No string type, just returning an empty string."
#define CMO_ERR_NO_FILE "No file type, just returning an empty string."
#define CMO_ERR_OOR "Out of range, use count() to check the number of given options."

enum CommandLineOptionType {
  CL_TYPE_STRING,
  CL_TYPE_LIT,
  CL_TYPE_INTEGER,
  CL_TYPE_DOUBLE,
  CL_TYPE_FILE,
  CL_TYPE_END
};

enum CommandLineNumArguments {
  CL_ZERO_OR_ONCE,
  CL_ONCE,
  CL_ANY
};

struct CommandLineOption {
  CommandLineOption();
  ~CommandLineOption();
  int count();                                                                        /* is possible to pass multiple values for one flag, this returns the number of passed elements */

  std::string getString(int index = 0);                                               /* used for CL_TYPE_STRING, returns the passed string */
  std::string getFileName(int index = 0);                                             /* used for CL_TYPE_FILE, returns the file value as passed on the command line */
  std::string getBaseName(int index = 0);                                             /* used for CL_TYPE_FILE, returns the basename (file w/o path) */
  std::string getExtension(int index = 0);                                            /* used for CL_TYPE_FILE, returns the extension of the file passed from the command line */
  double getDouble(int index = 0);                                                    /* used for CL_TYPE_DOUBLE, returns the double value */
  int getInt(int index = 0);                                                          /* used for CL_TYPE_INTEGER, returns the given integer value */
  bool isSet();                                                                       /* used for CL_TYPE_LIT, check if the flag has been passed to the application */

  CommandLineOptionType type;                                                         /* the type of this option, internally used to check if we can return the the values asked.. */
  void* arg;                                                                          /* pointer to the argtable2 type */ 
};

class CommandLineOptions {
 public:
  CommandLineOptions();
  ~CommandLineOptions();

  CommandLineOption* addString(const char* shortflag,                                       /* add a string option */
                               const char* longflag = NULL, 
                               const char* message = NULL, 
                               CommandLineNumArguments num = CL_ZERO_OR_ONCE,
                               int mincount = 1,
                               int maxcount = 5);

  CommandLineOption* addFlag(const char* shortflag,                                        /* add a basic flag to enable/disable different parts in your application */
                             const char* longflag = NULL, 
                             const char* message = NULL, 
                             CommandLineNumArguments num = CL_ZERO_OR_ONCE,
                             int mincount = 1,
                             int maxcount = 5);


  CommandLineOption* addDouble(const char* shortflag,                                       /* add a double type */
                               const char* longflag = NULL, 
                               const char* message = NULL, 
                               CommandLineNumArguments num = CL_ZERO_OR_ONCE,
                               int mincount = 1,
                               int maxcount = 5);

  CommandLineOption* addInt(const char* shortflag,                                          /* add an integer option */
                            const char* longflag = NULL, 
                            const char* message = NULL, 
                            CommandLineNumArguments num = CL_ZERO_OR_ONCE,
                            int mincount = 1,
                            int maxcount = 5);


  CommandLineOption* addFile(const char* shortflag,                                         /* add a file, so you can get the full path, extension and base name */
                             const char* longflag = NULL, 
                             const char* message = NULL, 
                             CommandLineNumArguments num = CL_ZERO_OR_ONCE,
                             int mincount = 1,
                             int maxcount = 5);


  bool parse(int argc, char* argv[]);                                                 /* parse arguments passed to you application */

  void print();                                                                       /* print all options to stdout */

 private:
  bool setup();
  
 private:
  bool is_setup;
  std::vector<CommandLineOption*> options;
  void** argtable;
};

#endif

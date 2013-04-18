#include <cmo/CommandLineOptions.h>
#include <roxlu/core/Log.h>

CommandLineOption::CommandLineOption() 
  :arg(NULL)
{
}

CommandLineOption::~CommandLineOption() {
  arg = NULL;
}

int CommandLineOption::count() {
  if(arg) {

    if(type == CL_TYPE_STRING) {
      return ((struct arg_str*)(arg))->count;
    }
    else if(type == CL_TYPE_DOUBLE) {
      return ((struct arg_dbl*)(arg))->count;
    }
    else if(type == CL_TYPE_INTEGER) {
      return ((struct arg_int*)(arg))->count;
    }
    else if(type == CL_TYPE_FILE) {
      return ((struct arg_file*)(arg))->count;
    }

  }
  return 0;
}

bool CommandLineOption::isSet() {
  if(arg) {
    return count();
  }
  return false;
}

std::string CommandLineOption::getString(int index) {
  if(type != CL_TYPE_STRING) {
    RX_ERROR(CMO_ERR_NO_STRING);
    return "";
  }

  if(index > count()) {
    RX_ERROR(CMO_ERR_OOR);
    return "";
  }

  struct arg_str* as = (struct arg_str*)(arg);
  std::string val(as->sval[index], strlen(as->sval[index]));
  return val;
}

std::string CommandLineOption::getFileName(int index) {
  if(type != CL_TYPE_FILE) {
    RX_ERROR(CMO_ERR_NO_FILE);
    return "";
  }

  if(index > count()) {
    RX_ERROR(CMO_ERR_OOR);
    return "";
  }

  struct arg_file* af = (struct arg_file*)(arg);
  std::string val(af->filename[index], strlen(af->filename[index]));
  return val;
}

std::string CommandLineOption::getBaseName(int index) {
  if(type != CL_TYPE_FILE) {
    RX_ERROR(CMO_ERR_NO_FILE);
    return "";
  }

  if(index > count()) {
    RX_ERROR(CMO_ERR_OOR);
    return "";
  }

  struct arg_file* af = (struct arg_file*)(arg);
  std::string val(af->basename[index], strlen(af->basename[index]));
  return val;
}

std::string CommandLineOption::getExtension(int index) {
  if(type != CL_TYPE_FILE) {
    RX_ERROR(CMO_ERR_NO_FILE);
    return "";
  }

  if(index > count()) {
    RX_ERROR(CMO_ERR_OOR);
    return "";
  }

  struct arg_file* af = (struct arg_file*)(arg);
  std::string val(af->extension[index], strlen(af->extension[index]));
  return val;
}

double CommandLineOption::getDouble(int index) {
  if(type != CL_TYPE_DOUBLE) {
    RX_ERROR(CMO_ERR_NO_DOUBLE);
    return 0.0;
  }

  if(index > count()) {
    RX_ERROR(CMO_ERR_OOR);
    return 0.0;
  }
  
  struct arg_dbl* ad = (struct arg_dbl*)(arg);
  return ad->dval[index];
}

int CommandLineOption::getInt(int index) {
  if(type != CL_TYPE_INTEGER) {
    RX_ERROR(CMO_ERR_NO_INTEGER);
    return 0;
  }

  if(index > count()) {
    RX_ERROR(CMO_ERR_OOR);
    return 0.0;
  }

  struct arg_int* ai = (struct arg_int*)(arg);
  return ai->ival[index];
}

// ---------------------------------------------

CommandLineOptions::CommandLineOptions() 
  :is_setup(false)
{
}

CommandLineOptions::~CommandLineOptions() {
  if(is_setup) {
    arg_freetable(argtable, options.size());
  }

  for(size_t i = 0; i < options.size(); ++i) {
    delete options[i];
  }

  is_setup = false;
}


CommandLineOption* CommandLineOptions::addString(const char* shortflag,
                                                 const char* longflag,
                                                 const char* message,
                                                 CommandLineNumArguments num,
                                                 int mincount,
                                                 int maxcount)
{
  CommandLineOption* option = new CommandLineOption();
  option->type = CL_TYPE_STRING;
  
  struct arg_str* arg = NULL;

  if(num == CL_ONCE) {
    arg = arg_str1(shortflag, longflag, NULL, message);
  }
  else if(num == CL_ANY) {
    arg = arg_strn(shortflag, longflag, NULL, mincount, maxcount, message);
  }
  else if(num == CL_ZERO_OR_ONCE) {
    arg = arg_str0(shortflag, longflag, NULL, message);
  }

  if(!arg) {
    RX_ERROR(CMO_ERR_ARG_ERR);
    delete option;
    return NULL;
  }

  option->arg = (void*)arg;

  options.push_back(option);

  return option;
}

CommandLineOption* CommandLineOptions::addFlag(const char* shortflag, 
                                               const char* longflag, 
                                               const char* message, 
                                               CommandLineNumArguments num,
                                               int mincount,
                                               int maxcount) 
{
  CommandLineOption* option = new CommandLineOption();
  option->type = CL_TYPE_LIT;
  
  struct arg_lit* arg = NULL;

  if(num == CL_ONCE) {
    arg = arg_lit1(shortflag, longflag, message);
  }
  else if(num == CL_ANY) {
    arg = arg_litn(shortflag, longflag, mincount, maxcount, message);
  }
  else if(num == CL_ZERO_OR_ONCE) {
    arg = arg_lit0(shortflag, longflag, message);
  }

  if(!arg) {
    RX_ERROR(CMO_ERR_ARG_ERR);
    delete option;
    return NULL;
  }

  option->arg = (void*)arg;

  options.push_back(option);

  return option;
}

CommandLineOption* CommandLineOptions::addInt(const char* shortflag,
                                              const char* longflag,
                                              const char* message,
                                              CommandLineNumArguments num,
                                              int mincount,
                                              int maxcount)
{
  CommandLineOption* option = new CommandLineOption();
  option->type = CL_TYPE_INTEGER;
  
  struct arg_int* arg = NULL;

  if(num == CL_ONCE) {
    arg = arg_int1(shortflag, longflag, NULL, message);
  }
  else if(num == CL_ANY) {
    arg = arg_intn(shortflag, longflag, NULL, mincount, maxcount, message);
  }
  else if(num == CL_ZERO_OR_ONCE) {
    arg = arg_int0(shortflag, longflag, NULL, message);
  }

  if(!arg) {
    RX_ERROR(CMO_ERR_ARG_ERR);
    delete option;
    return NULL;
  }

  option->arg = (void*)arg;

  options.push_back(option);

  return option;
}

CommandLineOption* CommandLineOptions::addDouble(const char* shortflag, 
                                                 const char* longflag, 
                                                 const char* message, 
                                                 CommandLineNumArguments num,
                                                 int mincount,
                                                 int maxcount) 
{
  CommandLineOption* option = new CommandLineOption();
  option->type = CL_TYPE_DOUBLE;
  
  struct arg_dbl* arg = NULL;

  if(num == CL_ONCE) {
    arg = arg_dbl1(shortflag, longflag, NULL, message);
  }
  else if(num == CL_ANY) {
    arg = arg_dbln(shortflag, longflag, NULL, mincount, maxcount, message);
  }
  else if(num == CL_ZERO_OR_ONCE) {
    arg = arg_dbl0(shortflag, longflag, NULL,  message);
  }

  if(!arg) {
    RX_ERROR(CMO_ERR_ARG_ERR);
    delete option;
    return NULL;
  }

  option->arg = (void*)arg;

  options.push_back(option);

  return option;
}

CommandLineOption* CommandLineOptions::addFile(const char* shortflag, 
                                               const char* longflag, 
                                               const char* message, 
                                               CommandLineNumArguments num,
                                               int mincount,
                                               int maxcount) 
{
  CommandLineOption* option = new CommandLineOption();
  option->type = CL_TYPE_FILE;
  
  struct arg_file* arg = NULL;

  if(num == CL_ONCE) {
    arg = arg_file1(shortflag, longflag, NULL, message);
  }
  else if(num == CL_ANY) {
    arg = arg_filen(shortflag, longflag, NULL, mincount, maxcount, message);
  }
  else if(num == CL_ZERO_OR_ONCE) {
    arg = arg_file0(shortflag, longflag, NULL,  message);
  }

  if(!arg) {
    RX_ERROR(CMO_ERR_ARG_ERR);
    delete option;
    return NULL;
  }

  option->arg = (void*)arg;

  options.push_back(option);

  return option;
}

bool CommandLineOptions::parse(int argc, char* argv[]) {
  if(!is_setup) {
    if(!setup()) {
      return false;
    }
  }

  if(arg_nullcheck(argtable) != 0) {
    RX_ERROR(CMO_ERR_NULLCHECK);
    return false;
  }

  int nerrors = arg_parse(argc, argv, argtable);
  if(nerrors > 0 ) {
    arg_print_errors(stdout, (struct arg_end*)options.back()->arg, "ERROR");
    return false;
  }

  return true;
}

void CommandLineOptions::print() {
  if(!is_setup) {
    return;
  }
  arg_print_glossary(stdout, argtable, "%-25s %s\n");
}

bool CommandLineOptions::setup() {
  if(!options.size()) {
    RX_ERROR(CMO_ERR_NO_OPTIONS);
    return false;
  }

  CommandLineOption* end = new CommandLineOption();
  end->type = CL_TYPE_END;
  end->arg = arg_end(20);
  options.push_back(end);

  argtable = new void*[options.size()];
  for(size_t i = 0; i < options.size(); ++i) {
    argtable[i] = (void*)options[i]->arg;
  }

  is_setup = true;
  return true;
}

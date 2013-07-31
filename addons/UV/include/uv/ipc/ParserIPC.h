#ifndef ROXLU_PARSER_IPC_H
#define ROXLU_PARSER_IPC_H

/*
  # ParserIPC
  
  This class is used to parse a buffer that follows the IPC protocol 
  that is used by the ServerIPC, ConnectionIPC and ClientIPC. 


 */

#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <uv/ipc/TypesIPC.h>

#define PIPC_STATE_COMMAND_SIZE 0
#define PIPC_STATE_COMMAND_READ 1
#define PIPC_STATE_DATA_SIZE 2
#define PIPC_STATE_DATA_READ 3 

typedef void(*parser_ipc_callback)(std::string path, char* data, size_t nbytes, void* user);

class ParserIPC {
 public:
  ParserIPC();
  ~ParserIPC();
  void parse(std::vector<char>& buffer);  /* parse! */
 public:
  uint32_t parse_state;                   /* the current parse state STATE_*_{SIZE, READ} */
  uint32_t data_size;                     /* the size of the data part for the current state */
  std::string parsed_method;              /* the method that we parsed (e.g. /call, /encode, /add_audio) */

  parser_ipc_callback cb_parser;
  void* cb_user;
};

#endif

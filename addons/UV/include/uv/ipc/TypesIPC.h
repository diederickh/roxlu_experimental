#ifndef ROXLU_IPC_TYPES_H
#define ROXLU_IPC_TYPES_H

#include <roxlu/core/Utils.h>
#include <string>

#define METHOD_IPC_COMMAND 1010           /* when executing a method this is used to identify the command from the buffer */

typedef void(*ipc_callback)(std::string path, char* data, size_t nbytes, void* user);

struct MethodIPC {
  
  MethodIPC();
  ~MethodIPC();

  ipc_callback cb_path;
  void* cb_user;  
  std::string path;
  uint32_t id;                          /* audo generated id, based on the path */
};

#endif

#include <uv/ipc/TypesIPC.h>

MethodIPC::MethodIPC()
  :cb_path(NULL)
  ,cb_user(NULL) 
{
}

MethodIPC::~MethodIPC() { 
  cb_path = NULL;
  cb_user = NULL;
  path = "";
}

#include <uv/ipc/ParserIPC.h>

ParserIPC::ParserIPC() 
  :parse_state(PIPC_STATE_COMMAND_SIZE)
  ,data_size(0)
  ,cb_parser(NULL)
  ,cb_user(NULL)
{
}

ParserIPC::~ParserIPC() {
  parse_state = 0;
  data_size = 0;
  parsed_method = "";
  cb_parser = NULL;
  cb_user = NULL;
}


// [ {4} cmd ] [ {4} path_len ] [ {path_len} path ] [ {4} data_len ] [ {data_len} data ]

void ParserIPC::parse(std::vector<char>& buffer) {
  while(buffer.size()) {
    switch(parse_state) {

      case PIPC_STATE_COMMAND_SIZE: {

        if(buffer.size() > 8)  {
          uint32_t cmd = 0;
          memcpy((char*)&cmd, &buffer[0], sizeof(cmd));

          if(cmd != METHOD_IPC_COMMAND) {
            RX_ERROR("Parsing command, but the received data is not a command - this shouldnt happen");
            return;
          }

          memcpy((char*)&data_size, &buffer[sizeof(cmd)], 4);
          parse_state = PIPC_STATE_COMMAND_READ;
          buffer.erase(buffer.begin(), buffer.begin() + 8);
          break;
        }

        return;
      }

      case PIPC_STATE_COMMAND_READ: {

        if(buffer.size() >= data_size) {
          parsed_method.clear();
          parsed_method.assign(&buffer[0], data_size);
          buffer.erase(buffer.begin(), buffer.begin() + data_size);
          parse_state = PIPC_STATE_DATA_SIZE;
          break;
        }

        return;
      }

      case PIPC_STATE_DATA_SIZE: {
        if(buffer.size() >= 4) {        
          memcpy((char*)&data_size, &buffer[0], 4);
          buffer.erase(buffer.begin(), buffer.begin() + 4);
          parse_state = PIPC_STATE_DATA_READ;
          break;
        }
        return;
      }

      case PIPC_STATE_DATA_READ: {
        if(buffer.size() >= data_size) {
          if(cb_parser) {
            cb_parser(parsed_method, data_size ? &buffer[0] : NULL, data_size, cb_user);
          }

          buffer.erase(buffer.begin(), buffer.begin() + data_size);
          parse_state = PIPC_STATE_COMMAND_SIZE;
          break;
        }
        else {

        }
        return;
      }

      default: {
        RX_VERBOSE("Unhandled parse_state.");
        return;
      }
    }
  }

}


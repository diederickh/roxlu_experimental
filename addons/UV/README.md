# UV

A couple of simple c++ wrappers around libuv. We implement a basic tcp client and server, 
see `ServerSocket` and `ClientSocket`. The `ServerIPC` and `ClientIPC` can be used for 
inter process communication using a unix domain socket.

## ServerIPC and ClientIPC

These two classes let you use unix domain sockets / named pipes in a client/server like fashion.
Together with msgpack or some other serialization library. When using these classes in a cross
platform fashion, make sure that windows has it's own naming standards for pipes.

_Unices_
````c++
std::string socket_file_name = "/tmp/myapp.sock";
````

_Win32_
````c++
std::string socket_file_name = "\\\\.\\pipe\\myapp"; // "myapp" is the custom name that you can change
````

### TODO
- Make the way we handle incoming data in the server connections and clients similar; I think
  it's a good choice to keep a `std::vector<char>` buffer for each client connection (server/client side)
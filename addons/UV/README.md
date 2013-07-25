# UV

A couple of simple c++ wrappers around libuv. We implement a basic tcp client and server, 
see `ServerSocket` and `ClientSocket`. The `ServerIPC` and `ClientIPC` can be used for 
inter process communication using a unix domain socket.



### TODO
- Make the way we handle incoming data in the server connections and clients similar; I think
  it's a good choice to keep a `std::vector<char>` buffer for each client connection (server/client side)
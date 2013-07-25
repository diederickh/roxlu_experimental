# MsgPack

This is an empty addon which adds the correct compiler 
settings for [msgpack](http://msgpack.org/)

_Example how to serialize/deserialize a struct_

````c++
#include <msgpack.hpp>

struct mytype {
       int id;
       std::string name;
       MSGPACK_DEFINE(id, name);
};

int main() {
    // create simple object
    mytype m;
    m.id = 10;
    m.name = "roxlu"

    // serialize
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, m);   

    // deserialize
    msgpack::unpacked unp;
    msgpack::unpack(&unp, sbuf.data(), sbuf.size());
    
    msgpack::object obj = unp.get();
    mytype deserialized;
    obj.convert(&deserialized);
    
    return 0;
}
````


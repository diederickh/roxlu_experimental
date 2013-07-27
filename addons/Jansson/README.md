# Jansson

Very basic wrapper around [libjansson](http://www.digip.org/jansson/doc/2.4/index.html) to read JSON
elements. I use this library to decode 3rd party services which use json or for configuration files.
This addons provides a basic json readers that uses path string to access the correct element.
Arrays are not supported at this moment.

_Example json_
````json
{
        "username":"roxlu",
        "app": {
               "frame_num" : 10,
               "auth_code" : { 
                           "secret" : "####"
               }

        }       
}
````
````c++ 
#include <jansson/Jansson.h>

Jansson j;
j.load("somefile.json", true);

std::string username;
std::string frame_path;        
int frame_num;           
std::string secret;

if(!j.getString("/username", username)) {
  return false;                             
}

if(!j.getInt("/app/frame_num", frame_num)) {
  return false;
}

if(!j.getString("/app/auth_code/secret", secret)) { 
  return false;
}
````
Command Line Options
--------------------

Simple wrapper around [argtable](http://argtable.sourceforge.net/), to add command line
options to your application. I use this when I need to load different configuration files
for debug and production applications.

_Simple usage (pseudo code):_
```C++

#include <cmo/CommandLineOptions.h>

int main(int argc, char* argv[]) {

    CommandLineOptions cmo;

    CommandLineOption* make_verbose = cmo.addFlag("v", "verbose", "Enable verbose output");
    CommandLineOption* config_file = cmo.addFile("f", "file", "Configuration file", GL_ONCE); // the configuration file is obligatory
    CommandLineOption* fps = cmo.addInt("r", "fps", "Framerate");

    if(!cmo.parse(argc, argv)) {
       printf("Error\n");
       return 0;
    }    

    // print options:
    cmo.print();

    if(make_verbose->isset()) {
        printf("Verbose is set.\n");
    }

    if(config_file->count()) {
        printf("File: %s, extension: %s\n", config_file->getFileName().c_str(), config_file->getExtension().c_str());                             
    }

}    
```        

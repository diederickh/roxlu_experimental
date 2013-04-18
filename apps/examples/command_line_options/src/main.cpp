#include <iostream>
#include <cmo/CommandLineOptions.h>

int main(int argc, char* argv[]) {

  CommandLineOptions cmo;

  // Add the options
  CommandLineOption* file = cmo.addString("f", "file", "Configuration filename, in datapath", CL_ONCE); // GL_ONCE makes sure that parse() returns false when this option is not given
  CommandLineOption* turn = cmo.addString("t", "turn", "Turn values");
  CommandLineOption* verbose = cmo.addString("v", "verbose", "Verbose output");
  CommandLineOption* devug_flag = cmo.addFlag("d","debug", "Run in debug mode");
  CommandLineOption* double_value = cmo.addDouble("p", "pi", "The value of pi", CL_ONCE);
  CommandLineOption* int_value = cmo.addInt("i", "integer_value", "Number of iterations");
  CommandLineOption* export_file = cmo.addFile("e", "export", "Export to");

  // parse() returns false, when the number of arguments for any option is not given. GL_ONCE means that at least one value must be given.
  if(!cmo.parse(argc, argv)) {
    printf("Error occured. Not all required options were set.\n");
    return 0;
  }

  // Here we print verbose info.
  cmo.print();

  // Check if the flag is given: ./this_file -v
  if(verbose->isSet()) {
    printf("> Verbose option is set.\n");
  }

  printf("> Number of double values: %d\n", double_value->count());
  if(double_value->isSet()) {
    printf("> Double value: %f\n", double_value->getDouble());
  }

  // Get some info for the given file.
  if(export_file->count()) {
    printf("Filename: %s, Basename: %s, Extension: %s\n", 
           export_file->getFileName().c_str(), 
           export_file->getBaseName().c_str(), 
           export_file->getExtension().c_str()
           );
  }

  return 1;
};

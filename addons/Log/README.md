Logger
------

The logger addons works together with the log macros that are used by the 
roxlu library. By default the roxlu library uses `RX_ERROR()`, `RX_WARNING`
and `RX_VERBOSE` to log messages.  Using global preprocessor flags it's possible
to remove all the log macros/function calls from your code if you want to make
a clean release version of your application.

You can change the log level dynamically using `rx_log_set_level(level);` passing one
of the following values for the level argument. `RX_LOG_LEVEL_VERBOSE`,`RX_LOG_LEVEL_WARNING`, 
`RX_LOG_LEVEL_ERROR`.

Or you can remove logging code by setting a global preprocessor flag (`RX_LOG_LEVEL`) 
to one of the log levels.

Note that the log macros, `RX_VERBOSE`, `RX_WARNING` and `RX_ERROR` can be used like `printf()`, 
so you can do: `RX_VERBOSE("Number of items: %d", 10)`.

_Example_

````C++
#include <log/Log.h>

roxlu::Log log;
log.setup("logs", "");

RX_VERBOSE("Log something to a file and console");

// --------------------------------------

log.mini(); // change output to minimal

RX_VERBOSE("Minimal log info");

// --------------------------------------

log.maxi(); // change output to "maximum"

RX_VERBOSE("Maximum info")

// --------------------------------------

log.writeToFile(false);     // change outputs
log.writeToConsole(true);   // chagne outputs

````

Logger
------

This example shows how you can use the log features of `roxlu_lib` together with the Log addon to add some more features.
The `Log` addon logs messages to a colored output and to a file with log rotate support.
By default it wil rotate every 1MB of logging data.

The log features of `roxlu_lib` are implemented using macros so you can completely 
remove them automatically from your source code if necessary. By default we leave all 
log macros/functions in your code. 

You can change the log level dynamically using `rx_log_set_level([RX_LOG_LEVEL_VERBOSE, RX_LOG_LEVEL_WARNING, RX_LOG_LEVEL_ERROR])`.

Or you can remove logging code by setting a global preprocessor flag (`RX_LOG_LEVEL`) to one of the  log levels.

See `build\cmake\CMakeLists.txt` where we set the `RX_LOG_LEVEL_ERROR` as log level
to remove all calls to `RX_VERBOSE` and `RX_WARNING`.

Note that the log macros, `RX_VERBOSE`, `RX_WARNING` and `RX_ERROR` can be used like `printf()`, 
so you can do: `RX_VERBOSE("Number of items: %d", 10)`.

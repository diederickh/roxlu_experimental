Logger
------

This example shows how you can use the log features of roxlu_lib with the Log addon.
The `Log` addon logs messages to a colored output and to a file with log rotate support.
By default it wil rotate every 1MB of logging data.

The log features of `roxlu_lib` are implemented using macros so that you can completely 
remove them automatically from your source code if necessary. By default we leave all 
log macros/functions in your code. 

You can change the log level dynamically using `rx_log_set_level([RX_LOG_LEVEL_VERBOSE, RX_LOG_LEVEL_WARNING, RX_LOG_LEVEL_ERROR])`.

Or you can remove all code by setting a compiler global preprocessor flags to `RX_LOG_LEVEL_NONE`. 

See `build\cmake\CMakeLists.txt` where you can set the `RX_LOG_LEVEL_ERROR` as global preprocessor
to remove all calls to `RX_VERBOSE` and `RX_WARNING`.
Logging
================================================================================


We are using the Google-Glog library for logging. The documentation of this library
is not good but the library has some nice features for logging and has a nice 
failure signal handler.

.. code-block:: c++

	// Set destination for files. Set this before initializing
        FLAGS_log_dir = "./log/";

        // Initialize with application name.
        google::InitGoogleLogging("Application");

        // If you want, you can set the logfile destination per level:
        // google::SetLogDestination(google::WARNING,"warning.log");

        // Log backtrace on crash:
        google::InstallFailureSignalHandler();


Some log examples:

.. code-block:: c++

        LOG(INFO) << "Some info to log";
        LOG(WARNING) << "Warning level log message.";
        LOG(ERROR) << "Error level";
        LOG(FATAL) << "This will stop execution of the application and logs a backtrace";

        VLOG(0) << "Log custom level, 0";
        VLOG(1) << "Log custom level, 1"; // etc...

        // If you include "stl_logging.h" you can log vectors too:
        vector<int> numbers;
        numbers.push_back(10);
        numbers.push_back(20);
        numbers.push_back(30);

        LOG(INFO) << "Numbers: " << numbers;

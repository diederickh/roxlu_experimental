# Utils

Code which I can't really organize in one of the other addons so this is just
a collections of files/classes/functions which I use for several applications.

## WindowsService

This piece of code registers a windows service. The awesome thing about a windows 
service is that it's installed in the services manager which will automatically
make sure that the service will be started again if it crashes.

Creating and using services can reduce the risk that your application crashes by
separating logic into separate processes. To create a process you need to create 
3 functions, one which gets called when your application starts, one gets called
continously in a loop, until the user decides to stop the event through the services
manager, and thirdly a function that gets called once to shutdown your service.

````c++

YouTube youtube;

// The service callbacks.
void yt_start(void* user) {
  RX_VERBOSE("Service starts");
  youtube.start();
}

void yt_run(void* user) {
  youtube.update();
}

void yt_stop(void* user) {
  RX_VERBOSE("Service stops");
  youtube.stop();
}

// Setup the service
rx_windows_service_setup("YouTubeUploader", yt_start, yt_run, yt_stop, NULL); // NULL, can be a user pointer whichs gets passed into the callbacks

// Start running
rx_windows_service_start();

````

#### Installing a service

Installing a service is really simple; Compile the executable, open a CMD window and  
enter this command. (note that the space after binPath= is necessary):  
Change YouTubeUploader to the name of our service.

````sh
C:\>sc create "YouTubeUploader" binPath= "C:\projects\youtubeuploader\youtubeuploader.exe"
````

#### Removing a service

````sh
C:\> sc delete "YouTubeUploader"
````

#### Starting a service from command line

````sh
C:\>net start "YouTubeUploader"
````

#### Stopping a service from command line

````sh
C:\>net stop "YouTubeUploader"
````

## SignalHandler

The signalhandler lets you register one or more signal handlers, which is handy when 
you want to capture crashes in you application and e.g. notify someone. You need to
create an instanceof `SignalHandler`, then set the callbacks: `cb_abrt`, `cb_fpe`, `cb_ill`, etc..
and then call `setup()`. If you want to use one signal callback for all signal types, you only need 
to set `cb_all`. If you want any specific user data to be passed to your callback, set `cb_user`.

````c++

void my_all_handler(int signum, void* user) {
  RX_VERBOSE("Got signal: %d", signum);     
}

int main() {
    SignalHandler sighandler;
    sighandler.cb_all = my_all_handler;
    sighandler.setup();
};

````
#include <roxlu/core/Log.h>
#include <utils/WindowsService.h>
#include <Strsafe.h>

WindowsService rx_win_service;

void rx_windows_service_setup(std::string title,
                              rx_win_service_callback startCB, 
                              rx_win_service_callback runCB,
                              rx_win_service_callback stopCB,
                              void* user)
{
  rx_win_service.title = title;
  rx_win_service.cb_start = startCB;
  rx_win_service.cb_update = runCB;
  rx_win_service.cb_stop = stopCB;
  rx_win_service.cb_user = user;
}

bool rx_windows_service_start() {
  SERVICE_TABLE_ENTRY sv_table[] = {
    {(LPSTR)rx_win_service.title.c_str(), (LPSERVICE_MAIN_FUNCTION)windows_service_main},
    {NULL, NULL}
  };

  if(StartServiceCtrlDispatcher(sv_table) == FALSE) {
    RX_ERROR("Cannot start the service");
    return false;
  }
  return true;
}

// ---------------------------------------------

VOID WINAPI windows_service_main(DWORD argc, LPTSTR* argv) {

  // Register the service with SCM
   rx_win_service.status_handle = RegisterServiceCtrlHandler(rx_win_service.title.c_str(), windows_service_ctrl_handler);
  if(rx_win_service.status_handle == NULL) {
    RX_ERROR("Cannot register the service");
    return;
  }

  // Initialize the service struct
  ZeroMemory(&rx_win_service.status, sizeof(rx_win_service.status));
  rx_win_service.status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  rx_win_service.status.dwControlsAccepted = 0;
  rx_win_service.status.dwCurrentState = SERVICE_START_PENDING;
  rx_win_service.status.dwWin32ExitCode = 0;
  rx_win_service.status.dwServiceSpecificExitCode = 0;
  rx_win_service.status.dwCheckPoint = 0;
  
  if(SetServiceStatus(rx_win_service.status_handle, &rx_win_service.status) == FALSE) {
    RX_ERROR("Cannot set the service status");
  }

  // stop event
  rx_win_service.stop_event = CreateEvent(NULL,  // default security attributes
                               TRUE,             // manual reset event
                               FALSE,            // not signaled
                               NULL);            // no name

  if(rx_win_service.stop_event == NULL) {
    rx_win_service.status.dwControlsAccepted = 0;
    rx_win_service.status.dwCurrentState = SERVICE_STOPPED;
    rx_win_service.status.dwWin32ExitCode = GetLastError();
    rx_win_service.status.dwCheckPoint = 1;

    if(SetServiceStatus(rx_win_service.status_handle, &rx_win_service.status) == FALSE) {
      RX_ERROR("Cannot set the service status after failing to create the stop event");
    };

    return;
  }

  // start 
  rx_win_service.status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
  rx_win_service.status.dwCurrentState = SERVICE_RUNNING;
  rx_win_service.status.dwWin32ExitCode = 0;
  rx_win_service.status.dwCheckPoint = 0;

  if(SetServiceStatus(rx_win_service.status_handle, &rx_win_service.status) == FALSE) {
    RX_ERROR("Cannot set the status to `start`");
  }


#if 1
  if(rx_win_service.cb_start) {
    rx_win_service.cb_start(rx_win_service.cb_user);
  }

  if(rx_win_service.cb_update) {
    while(WaitForSingleObject(rx_win_service.stop_event, 0) != WAIT_OBJECT_0) {
      rx_win_service.cb_update(rx_win_service.cb_user);
    }
  }

  if(rx_win_service.cb_stop) {
    rx_win_service.cb_stop(rx_win_service.cb_user);
  }

#else 
  // perform the service task (in a separate thread)
  HANDLE thread_handle = CreateThread(NULL, 0, windows_service_thread, NULL, 0, NULL);
  WaitForSingleObject(thread_handle, INFINITE);
#endif

  // shutdown
  rx_win_service.status.dwControlsAccepted = 0;
  rx_win_service.status.dwCurrentState = SERVICE_STOPPED;
  rx_win_service.status.dwWin32ExitCode = 0;
  rx_win_service.status.dwCheckPoint = 3;

  if(SetServiceStatus(rx_win_service.status_handle, &rx_win_service.status) == FALSE) {
    RX_ERROR("Cannot set the status to `STOPPED`");
  }

}

DWORD WINAPI windows_service_thread(LPVOID lp) {

  rx_win_service.cb_start(rx_win_service.cb_user);

  while(WaitForSingleObject(rx_win_service.stop_event, 0) != WAIT_OBJECT_0) {
    rx_win_service.cb_update(rx_win_service.cb_user);
  }

  rx_win_service.cb_stop(rx_win_service.cb_user);
  return ERROR_SUCCESS;
}


VOID WINAPI windows_service_ctrl_handler(DWORD code) {
  switch(code) {
    case SERVICE_CONTROL_STOP: {

      if(rx_win_service.status.dwCurrentState != SERVICE_RUNNING) {
        RX_ERROR("Trying to stop the service, but we're not running.");
        return;
      }

      rx_win_service.status.dwControlsAccepted = 0;
      rx_win_service.status.dwCurrentState = SERVICE_STOP_PENDING;
      rx_win_service.status.dwWin32ExitCode = 0;
      rx_win_service.status.dwCheckPoint = 4;
      if(SetServiceStatus(rx_win_service.status_handle, &rx_win_service.status) == FALSE) {
        RX_ERROR("Cannot set the status to `STOPPED`");
      }

      SetEvent(rx_win_service.stop_event);

    };
    default: break;
  }
}


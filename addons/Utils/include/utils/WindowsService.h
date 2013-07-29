#ifndef ROXLU_WINDOWS_SERVICE_H
#define ROXLU_WINDOWS_SERVICE_H

#include <windows.h>
#include <string>

typedef void(*rx_win_service_callback)(void* user);

VOID WINAPI windows_service_main(DWORD argc, LPTSTR* argv);                      /* callback: service entry point */
VOID WINAPI windows_service_ctrl_handler(DWORD code);                            /* callback: handles control messages (like start/stopping) */
DWORD WINAPI windows_service_thread(LPVOID lp);                                  /* callback: not used atm; but separate thread in which the service can run */

void rx_windows_service_setup(std::string title,                                 /* initializes a service with the given title */               
                              rx_win_service_callback startCB,                   /* gets called once when the service starts  */
                              rx_win_service_callback updateCB,                  /* gets called repeadatly; should be non blocking! */ 
                              rx_win_service_callback stopCB,                    /* gets called once when when we stop the service */
                              void* user);                                       /* user data gets passed in the 3 callbacks */

bool rx_windows_service_start();                                                 /* registers the services and makes sure it's started when needed */

struct WindowsService {
  std::string title;
  SERVICE_STATUS status;
  SERVICE_STATUS_HANDLE status_handle;
  HANDLE stop_event;
  rx_win_service_callback cb_start;
  rx_win_service_callback cb_update;
  rx_win_service_callback cb_stop;
  void* cb_user;
};

extern WindowsService rx_win_service;

#endif

#include <utils/SignalHandler.h>

#define SIG_SET_ALL(fc) { if(!fc) { fc = cb_all; } } 
#define SIG_CALL(fc) { SignalHandler* sh = SignalHandler::instance; if(sh->fc) { sh->fc(signum, sh->cb_user); } }  

SignalHandler* SignalHandler::instance = NULL;

// ---------------------------------------------------------------------------

void signal_handler_abrt(int signum) {
  RX_ERROR("Received a signal: SIGABRT - abnormal termination of program");
  SIG_CALL(cb_abrt);
}

void signal_handler_fpe(int signum) {
  RX_ERROR("Received a signal: SIGFPE - arithmic error; maybe divide by zero");
  SIG_CALL(cb_fpe);
}

void signal_handler_ill(int signum) {
  RX_ERROR("Recieved a signal SIGILL - illegal instruction");
  SIG_CALL(cb_ill);
}

void signal_handler_int(int signum) {
  RX_ERROR("Recieved a signal SIGILL - interactive attention signal - CTRL + C");
  SIG_CALL(cb_int);
}

void signal_handler_segv(int signum) {
  RX_ERROR("Received a signal SIGSEGV - invalid access storage");
  SIG_CALL(cb_segv);
}

void signal_handler_term(int signum) {
  RX_ERROR("Recieved a signal SIGTERM - A termination request sent to the program");
  SIG_CALL(cb_term);
}

// ---------------------------------------------------------------------------

SignalHandler::SignalHandler() 
  :cb_abrt(NULL)
  ,cb_fpe(NULL)
  ,cb_ill(NULL)
  ,cb_int(NULL)
  ,cb_segv(NULL)
  ,cb_term(NULL)
  ,cb_all(NULL)
  ,cb_user(NULL)
{

  if(instance != NULL) {
    RX_ERROR("Trying to add multiple signal handlers; do not instantiate SignalHandler more then once!");
    return;
  }
  
  instance = this;
}

SignalHandler::~SignalHandler() {
  cb_abrt = NULL;
  cb_fpe = NULL;
  cb_ill = NULL;
  cb_int = NULL;
  cb_segv = NULL;
  cb_term = NULL;
  cb_all = NULL;
  cb_user = NULL;
}

void SignalHandler::setup() {
  SIG_SET_ALL(cb_abrt);
  SIG_SET_ALL(cb_fpe);
  SIG_SET_ALL(cb_ill);
  SIG_SET_ALL(cb_int);
  SIG_SET_ALL(cb_segv);
  SIG_SET_ALL(cb_term);
  
  signal(SIGABRT, signal_handler_abrt);
  signal(SIGFPE, signal_handler_fpe);
  signal(SIGILL, signal_handler_ill);
  signal(SIGINT, signal_handler_int);
  signal(SIGSEGV, signal_handler_segv);
  signal(SIGTERM, signal_handler_term);
}

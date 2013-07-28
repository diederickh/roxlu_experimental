#ifndef ROXLU_SIGNAL_HANDLER_H
#define ROXLU_SIGNAL_HANDLER_H

#include <roxlu/core/Log.h>
#include <csignal>

typedef void(*signal_handler_callback)(int signum, void* user);

/* default signal handlers, which call RX_ERROR with a message */
void signal_handler_abrt(int signum);
void signal_handler_fpe(int signum);
void signal_handler_ill(int signum);
void signal_handler_int(int signum);
void signal_handler_segv(int signum);
void signal_handler_term(int signum);

class SignalHandler {
 public:
  SignalHandler();
  ~SignalHandler();
  void setup();                          /* install the set signal handler callbacks */

  signal_handler_callback cb_all;        /* this one is special; if set it will be used for all the unset handlers */

  signal_handler_callback cb_abrt;       /* abnormal termination */
  signal_handler_callback cb_fpe;        /* arithmic error (divide by zero) */
  signal_handler_callback cb_ill;        /* illegal instruction */
  signal_handler_callback cb_int;        /* interactive attention signal, ctrl + c */
  signal_handler_callback cb_segv;       /* segfault */
  signal_handler_callback cb_term;       /* termination request sent to the program */

  void* cb_user;                         /* user data, gets passed into the handlers */

  static SignalHandler* instance;
};

#endif

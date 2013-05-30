#ifndef ROXLU_CANON_TASK_CLOSE_SESSION_H
#define ROXLU_CANON_TASK_CLOSE_SESSION_H

#include <videocapture/edsdk/CanonTypes.h>

class CanonTaskCloseSession : public CanonTask {
 public:
  CanonTaskCloseSession(Canon* canon);
  ~CanonTaskCloseSession();
  bool execute();
};


#endif

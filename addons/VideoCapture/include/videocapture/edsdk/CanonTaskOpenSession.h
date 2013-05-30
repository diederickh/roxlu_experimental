#ifndef ROXLU_CANON_TASK_OPEN_SESSION_H
#define ROXLU_CANON_TASK_OPEN_SESSION_H

#include <videocapture/edsdk/CanonTypes.h>

class CanonTaskOpenSession : public CanonTask {
 public:
  CanonTaskOpenSession(Canon* canon);
  ~CanonTaskOpenSession();
  bool execute();
};


#endif

#ifndef ROXLU_CANON_TASK_STOP_H
#define ROXLU_CANON_TASK_STOP_H

#include <videocapture/edsdk/CanonTypes.h>

class CanonTaskStop : public CanonTask {
 public:
  CanonTaskStop(Canon* c);
  ~CanonTaskStop();
  bool execute();
};


#endif

#ifndef ROXLU_CANON_TASK_EVF_START_H
#define ROXLU_CANON_TASK_EVF_START_H

#include <videocapture/edsdk/CanonTypes.h>

class CanonTaskEvfStart : public CanonTask {
 public:
  CanonTaskEvfStart(Canon* canon);
  ~CanonTaskEvfStart();
  bool execute();
};

#endif

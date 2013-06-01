#ifndef ROXLU_CANON_TASK_EVF_END_H
#define ROXLU_CANON_TASK_EVF_END_H

#include <videocapture/edsdk/CanonTypes.h>

class CanonTaskEvfEnd : public CanonTask {
 public:
  CanonTaskEvfEnd(Canon* canon);
  ~CanonTaskEvfEnd();
  bool execute();
};


#endif

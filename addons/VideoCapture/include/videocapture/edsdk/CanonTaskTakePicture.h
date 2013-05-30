#ifndef ROXLU_CANON_TASK_TAKE_PICTURE_H
#define ROXLU_CANON_TASK_TAKE_PICTURE_H

#include <videocapture/edsdk/CanonTypes.h>

class CanonTaskTakePicture : public CanonTask {
 public:
  CanonTaskTakePicture(Canon* canon);
  ~CanonTaskTakePicture();
  bool execute();
  bool takePicture();
};


#endif

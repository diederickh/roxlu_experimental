#ifndef ROXLU_CANON_TASK_EVF_DOWNLOAD_H
#define ROXLU_CANON_TASK_EVF_DOWNLOAD_H

#include <videocapture/edsdk/CanonTypes.h>

class CanonTaskEvfDownload : public CanonTask {
 public:
  CanonTaskEvfDownload(Canon* canon);
  ~CanonTaskEvfDownload();
  bool execute();
};


#endif

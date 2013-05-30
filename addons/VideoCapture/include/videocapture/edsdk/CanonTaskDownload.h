#ifndef ROXLU_CANON_TASK_DOWNLOAD_H
#define ROXLU_CANON_TASK_DOWNLOAD_H

#include <videocapture/edsdk/CanonTypes.h>

class CanonTaskDownload : public CanonTask {
 public:
  CanonTaskDownload(Canon* canon, EdsDirectoryItemRef item);
  ~CanonTaskDownload();
  bool execute();
  bool takePicture();
 private:
  EdsDirectoryItemRef item;
};


#endif

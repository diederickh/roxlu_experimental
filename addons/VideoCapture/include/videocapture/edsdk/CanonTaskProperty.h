#ifndef ROXLU_CANON_TASK_PROPERTY_H
#define ROXLU_CANON_TASK_PROPERTY_H

#include <videocapture/edsdk/CanonTypes.h>

class CanonTaskProperty : public CanonTask {
 public:
  CanonTaskProperty(Canon* canon, EdsPropertyID propertyID);
  ~CanonTaskProperty();
  bool execute();
 private:
  bool getProperty(EdsPropertyID prop);
 public:
  EdsPropertyID property_id;
};


#endif

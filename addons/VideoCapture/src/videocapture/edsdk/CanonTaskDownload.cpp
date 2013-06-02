#include <roxlu/core/Utils.h>
#include <videocapture/edsdk/Canon.h>
#include <videocapture/edsdk/CanonUtils.h>
#include <videocapture/edsdk/CanonTaskDownload.h>

CanonTaskDownload::CanonTaskDownload(Canon* canon, EdsDirectoryItemRef item)
  :CanonTask(canon, CANON_TASK_DOWNLOAD)
  ,item(item)
{
  
}

CanonTaskDownload::~CanonTaskDownload() {
}

bool CanonTaskDownload::execute() {
  if(canon->isLegacy()) {
    canon->lockUI();
  }

  EdsError err = EDS_ERR_OK;
  EdsStreamRef stream = NULL;
  EdsDirectoryItemInfo dir_item_info;
  bool result = true;
  
  err = EdsGetDirectoryItemInfo(item, &dir_item_info);
  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
    result = false;
    goto done;
  }

  err = EdsCreateFileStream(dir_item_info.szFileName, kEdsFileCreateDisposition_CreateAlways, kEdsAccess_ReadWrite, &stream);
  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
    result = false;
    goto done;
  }

  err = EdsSetProgressCallback(stream, canon_download_progress, kEdsProgressOption_Periodically, (EdsVoid*) this);
  if(err != EDS_ERR_OK) {
    CANON_ERROR(err);
    result = false;
    goto done;
  }

  err = EdsDownload(item, dir_item_info.size, stream);
  if(err == EDS_ERR_OK) {
    err = EdsDownloadComplete(item);
    if(err != EDS_ERR_OK) {
      CANON_ERROR(err);
      result = false;
      goto done;
    }
  }
  else {
    CANON_ERROR(err);
  }

 done:
  if(canon->isLegacy()) {
    canon->unlockUI();
  }

  if(stream) {
    EdsRelease(stream);
    stream = NULL;
  }
  return result;
}


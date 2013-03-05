
#ifndef glxw_glx_h
#define glxw_glx_h

struct glxw_glx;

#include <GL/glx.h>
#include <GL/glxext.h>


#ifndef __gl_h_
#define __gl_h_
#endif

#ifdef __cplusplus
extern "C" {
#endif

int glxwInitGLX(void);
int glxwInitGLXCtx(struct glxw_glx *ctx);

struct glxw_glx {
PFNGLXGETFBCONFIGATTRIBPROC _glXGetFBConfigAttrib;
PFNGLXCREATEWINDOWPROC _glXCreateWindow;
PFNGLXDESTROYWINDOWPROC _glXDestroyWindow;
PFNGLXCREATEPIXMAPPROC _glXCreatePixmap;
PFNGLXDESTROYPIXMAPPROC _glXDestroyPixmap;
PFNGLXCREATEPBUFFERPROC _glXCreatePbuffer;
PFNGLXDESTROYPBUFFERPROC _glXDestroyPbuffer;
PFNGLXQUERYDRAWABLEPROC _glXQueryDrawable;
PFNGLXCREATENEWCONTEXTPROC _glXCreateNewContext;
PFNGLXMAKECONTEXTCURRENTPROC _glXMakeContextCurrent;
PFNGLXGETCURRENTREADDRAWABLEPROC _glXGetCurrentReadDrawable;
PFNGLXQUERYCONTEXTPROC _glXQueryContext;
PFNGLXSELECTEVENTPROC _glXSelectEvent;
PFNGLXGETSELECTEDEVENTPROC _glXGetSelectedEvent;
PFNGLXCREATECONTEXTATTRIBSARBPROC _glXCreateContextAttribsARB;
PFNGLXSWAPINTERVALSGIPROC _glXSwapIntervalSGI;
PFNGLXGETVIDEOSYNCSGIPROC _glXGetVideoSyncSGI;
PFNGLXWAITVIDEOSYNCSGIPROC _glXWaitVideoSyncSGI;
PFNGLXMAKECURRENTREADSGIPROC _glXMakeCurrentReadSGI;
PFNGLXGETCURRENTREADDRAWABLESGIPROC _glXGetCurrentReadDrawableSGI;
PFNGLXQUERYCONTEXTINFOEXTPROC _glXQueryContextInfoEXT;
PFNGLXGETCONTEXTIDEXTPROC _glXGetContextIDEXT;
PFNGLXIMPORTCONTEXTEXTPROC _glXImportContextEXT;
PFNGLXFREECONTEXTEXTPROC _glXFreeContextEXT;
PFNGLXCREATEGLXPBUFFERSGIXPROC _glXCreateGLXPbufferSGIX;
PFNGLXDESTROYGLXPBUFFERSGIXPROC _glXDestroyGLXPbufferSGIX;
PFNGLXQUERYGLXPBUFFERSGIXPROC _glXQueryGLXPbufferSGIX;
PFNGLXSELECTEVENTSGIXPROC _glXSelectEventSGIX;
PFNGLXGETSELECTEDEVENTSGIXPROC _glXGetSelectedEventSGIX;
PFNGLXCUSHIONSGIPROC _glXCushionSGI;
PFNGLXBINDCHANNELTOWINDOWSGIXPROC _glXBindChannelToWindowSGIX;
PFNGLXCHANNELRECTSGIXPROC _glXChannelRectSGIX;
PFNGLXQUERYCHANNELRECTSGIXPROC _glXQueryChannelRectSGIX;
PFNGLXQUERYCHANNELDELTASSGIXPROC _glXQueryChannelDeltasSGIX;
PFNGLXCHANNELRECTSYNCSGIXPROC _glXChannelRectSyncSGIX;
PFNGLXJOINSWAPGROUPSGIXPROC _glXJoinSwapGroupSGIX;
PFNGLXBINDSWAPBARRIERSGIXPROC _glXBindSwapBarrierSGIX;
PFNGLXQUERYMAXSWAPBARRIERSSGIXPROC _glXQueryMaxSwapBarriersSGIX;
PFNGLXGETTRANSPARENTINDEXSUNPROC _glXGetTransparentIndexSUN;
PFNGLXCOPYSUBBUFFERMESAPROC _glXCopySubBufferMESA;
PFNGLXCREATEGLXPIXMAPMESAPROC _glXCreateGLXPixmapMESA;
PFNGLXRELEASEBUFFERSMESAPROC _glXReleaseBuffersMESA;
PFNGLXSET3DFXMODEMESAPROC _glXSet3DfxModeMESA;
PFNGLXGETSYNCVALUESOMLPROC _glXGetSyncValuesOML;
PFNGLXGETMSCRATEOMLPROC _glXGetMscRateOML;
PFNGLXSWAPBUFFERSMSCOMLPROC _glXSwapBuffersMscOML;
PFNGLXWAITFORMSCOMLPROC _glXWaitForMscOML;
PFNGLXWAITFORSBCOMLPROC _glXWaitForSbcOML;
PFNGLXHYPERPIPECONFIGSGIXPROC _glXHyperpipeConfigSGIX;
PFNGLXDESTROYHYPERPIPECONFIGSGIXPROC _glXDestroyHyperpipeConfigSGIX;
PFNGLXBINDHYPERPIPESGIXPROC _glXBindHyperpipeSGIX;
PFNGLXQUERYHYPERPIPEBESTATTRIBSGIXPROC _glXQueryHyperpipeBestAttribSGIX;
PFNGLXHYPERPIPEATTRIBSGIXPROC _glXHyperpipeAttribSGIX;
PFNGLXQUERYHYPERPIPEATTRIBSGIXPROC _glXQueryHyperpipeAttribSGIX;
PFNGLXBINDTEXIMAGEEXTPROC _glXBindTexImageEXT;
PFNGLXRELEASETEXIMAGEEXTPROC _glXReleaseTexImageEXT;
PFNGLXBINDVIDEODEVICENVPROC _glXBindVideoDeviceNV;
PFNGLXGETVIDEODEVICENVPROC _glXGetVideoDeviceNV;
PFNGLXRELEASEVIDEODEVICENVPROC _glXReleaseVideoDeviceNV;
PFNGLXBINDVIDEOIMAGENVPROC _glXBindVideoImageNV;
PFNGLXRELEASEVIDEOIMAGENVPROC _glXReleaseVideoImageNV;
PFNGLXSENDPBUFFERTOVIDEONVPROC _glXSendPbufferToVideoNV;
PFNGLXGETVIDEOINFONVPROC _glXGetVideoInfoNV;
PFNGLXJOINSWAPGROUPNVPROC _glXJoinSwapGroupNV;
PFNGLXBINDSWAPBARRIERNVPROC _glXBindSwapBarrierNV;
PFNGLXQUERYSWAPGROUPNVPROC _glXQuerySwapGroupNV;
PFNGLXQUERYMAXSWAPGROUPSNVPROC _glXQueryMaxSwapGroupsNV;
PFNGLXQUERYFRAMECOUNTNVPROC _glXQueryFrameCountNV;
PFNGLXRESETFRAMECOUNTNVPROC _glXResetFrameCountNV;
PFNGLXBINDVIDEOCAPTUREDEVICENVPROC _glXBindVideoCaptureDeviceNV;
PFNGLXLOCKVIDEOCAPTUREDEVICENVPROC _glXLockVideoCaptureDeviceNV;
PFNGLXQUERYVIDEOCAPTUREDEVICENVPROC _glXQueryVideoCaptureDeviceNV;
PFNGLXRELEASEVIDEOCAPTUREDEVICENVPROC _glXReleaseVideoCaptureDeviceNV;
PFNGLXSWAPINTERVALEXTPROC _glXSwapIntervalEXT;
PFNGLXCOPYIMAGESUBDATANVPROC _glXCopyImageSubDataNV;
};

extern struct glxw_glx *glxw_glx;

#define glXGetFBConfigAttrib (glxw_glx->_glXGetFBConfigAttrib)
#define glXCreateWindow (glxw_glx->_glXCreateWindow)
#define glXDestroyWindow (glxw_glx->_glXDestroyWindow)
#define glXCreatePixmap (glxw_glx->_glXCreatePixmap)
#define glXDestroyPixmap (glxw_glx->_glXDestroyPixmap)
#define glXCreatePbuffer (glxw_glx->_glXCreatePbuffer)
#define glXDestroyPbuffer (glxw_glx->_glXDestroyPbuffer)
#define glXQueryDrawable (glxw_glx->_glXQueryDrawable)
#define glXCreateNewContext (glxw_glx->_glXCreateNewContext)
#define glXMakeContextCurrent (glxw_glx->_glXMakeContextCurrent)
#define glXGetCurrentReadDrawable (glxw_glx->_glXGetCurrentReadDrawable)
#define glXQueryContext (glxw_glx->_glXQueryContext)
#define glXSelectEvent (glxw_glx->_glXSelectEvent)
#define glXGetSelectedEvent (glxw_glx->_glXGetSelectedEvent)
#define glXCreateContextAttribsARB (glxw_glx->_glXCreateContextAttribsARB)
#define glXSwapIntervalSGI (glxw_glx->_glXSwapIntervalSGI)
#define glXGetVideoSyncSGI (glxw_glx->_glXGetVideoSyncSGI)
#define glXWaitVideoSyncSGI (glxw_glx->_glXWaitVideoSyncSGI)
#define glXMakeCurrentReadSGI (glxw_glx->_glXMakeCurrentReadSGI)
#define glXGetCurrentReadDrawableSGI (glxw_glx->_glXGetCurrentReadDrawableSGI)
#define glXQueryContextInfoEXT (glxw_glx->_glXQueryContextInfoEXT)
#define glXGetContextIDEXT (glxw_glx->_glXGetContextIDEXT)
#define glXImportContextEXT (glxw_glx->_glXImportContextEXT)
#define glXFreeContextEXT (glxw_glx->_glXFreeContextEXT)
#define glXCreateGLXPbufferSGIX (glxw_glx->_glXCreateGLXPbufferSGIX)
#define glXDestroyGLXPbufferSGIX (glxw_glx->_glXDestroyGLXPbufferSGIX)
#define glXQueryGLXPbufferSGIX (glxw_glx->_glXQueryGLXPbufferSGIX)
#define glXSelectEventSGIX (glxw_glx->_glXSelectEventSGIX)
#define glXGetSelectedEventSGIX (glxw_glx->_glXGetSelectedEventSGIX)
#define glXCushionSGI (glxw_glx->_glXCushionSGI)
#define glXBindChannelToWindowSGIX (glxw_glx->_glXBindChannelToWindowSGIX)
#define glXChannelRectSGIX (glxw_glx->_glXChannelRectSGIX)
#define glXQueryChannelRectSGIX (glxw_glx->_glXQueryChannelRectSGIX)
#define glXQueryChannelDeltasSGIX (glxw_glx->_glXQueryChannelDeltasSGIX)
#define glXChannelRectSyncSGIX (glxw_glx->_glXChannelRectSyncSGIX)
#define glXJoinSwapGroupSGIX (glxw_glx->_glXJoinSwapGroupSGIX)
#define glXBindSwapBarrierSGIX (glxw_glx->_glXBindSwapBarrierSGIX)
#define glXQueryMaxSwapBarriersSGIX (glxw_glx->_glXQueryMaxSwapBarriersSGIX)
#define glXGetTransparentIndexSUN (glxw_glx->_glXGetTransparentIndexSUN)
#define glXCopySubBufferMESA (glxw_glx->_glXCopySubBufferMESA)
#define glXCreateGLXPixmapMESA (glxw_glx->_glXCreateGLXPixmapMESA)
#define glXReleaseBuffersMESA (glxw_glx->_glXReleaseBuffersMESA)
#define glXSet3DfxModeMESA (glxw_glx->_glXSet3DfxModeMESA)
#define glXGetSyncValuesOML (glxw_glx->_glXGetSyncValuesOML)
#define glXGetMscRateOML (glxw_glx->_glXGetMscRateOML)
#define glXSwapBuffersMscOML (glxw_glx->_glXSwapBuffersMscOML)
#define glXWaitForMscOML (glxw_glx->_glXWaitForMscOML)
#define glXWaitForSbcOML (glxw_glx->_glXWaitForSbcOML)
#define glXHyperpipeConfigSGIX (glxw_glx->_glXHyperpipeConfigSGIX)
#define glXDestroyHyperpipeConfigSGIX (glxw_glx->_glXDestroyHyperpipeConfigSGIX)
#define glXBindHyperpipeSGIX (glxw_glx->_glXBindHyperpipeSGIX)
#define glXQueryHyperpipeBestAttribSGIX (glxw_glx->_glXQueryHyperpipeBestAttribSGIX)
#define glXHyperpipeAttribSGIX (glxw_glx->_glXHyperpipeAttribSGIX)
#define glXQueryHyperpipeAttribSGIX (glxw_glx->_glXQueryHyperpipeAttribSGIX)
#define glXBindTexImageEXT (glxw_glx->_glXBindTexImageEXT)
#define glXReleaseTexImageEXT (glxw_glx->_glXReleaseTexImageEXT)
#define glXBindVideoDeviceNV (glxw_glx->_glXBindVideoDeviceNV)
#define glXGetVideoDeviceNV (glxw_glx->_glXGetVideoDeviceNV)
#define glXReleaseVideoDeviceNV (glxw_glx->_glXReleaseVideoDeviceNV)
#define glXBindVideoImageNV (glxw_glx->_glXBindVideoImageNV)
#define glXReleaseVideoImageNV (glxw_glx->_glXReleaseVideoImageNV)
#define glXSendPbufferToVideoNV (glxw_glx->_glXSendPbufferToVideoNV)
#define glXGetVideoInfoNV (glxw_glx->_glXGetVideoInfoNV)
#define glXJoinSwapGroupNV (glxw_glx->_glXJoinSwapGroupNV)
#define glXBindSwapBarrierNV (glxw_glx->_glXBindSwapBarrierNV)
#define glXQuerySwapGroupNV (glxw_glx->_glXQuerySwapGroupNV)
#define glXQueryMaxSwapGroupsNV (glxw_glx->_glXQueryMaxSwapGroupsNV)
#define glXQueryFrameCountNV (glxw_glx->_glXQueryFrameCountNV)
#define glXResetFrameCountNV (glxw_glx->_glXResetFrameCountNV)
#define glXBindVideoCaptureDeviceNV (glxw_glx->_glXBindVideoCaptureDeviceNV)
#define glXLockVideoCaptureDeviceNV (glxw_glx->_glXLockVideoCaptureDeviceNV)
#define glXQueryVideoCaptureDeviceNV (glxw_glx->_glXQueryVideoCaptureDeviceNV)
#define glXReleaseVideoCaptureDeviceNV (glxw_glx->_glXReleaseVideoCaptureDeviceNV)
#define glXSwapIntervalEXT (glxw_glx->_glXSwapIntervalEXT)
#define glXCopyImageSubDataNV (glxw_glx->_glXCopyImageSubDataNV)

#ifdef __cplusplus
}
#endif

#endif

#ifndef _VE_VIEWER_LISTENER_ANDROID_
#define _VE_VIEWER_LISTENER_ANDROID_
#include "KernelCore/ViewerListener.h"
#include <jni.h>

class veViewer;
class VE_EXPORT veViewerListenerAndroid : public veViewerListener
{
    friend class veViewerAndroid;
public:
    veViewerListenerAndroid();
    virtual  ~veViewerListenerAndroid();

    virtual void onCreated(veViewer *viewer, int width, int height) {}
    virtual void onDestroy(veViewer *viewer) {}
    virtual void onChanged(veViewer *viewer, int width, int height) {}
    virtual void onPause(veViewer *viewer) {}
    virtual void onResume(veViewer *viewer) {}

};


#endif

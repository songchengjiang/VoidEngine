#if (VE_PLATFORM == VE_PLATFORM_IOS)

#include "Viewer-ios.h"
#include "Application-ios.h"
#include "SceneManager.h"

#import "GLView.h"

veViewerIOS::veViewerIOS(int width, int height, const std::string &title, void *sharedView)
    : veViewer(width, height, title)
    , _sharedGLView(sharedView)
    , _glView(nullptr)
    , _isRendering(false)
{
}

veViewerIOS::~veViewerIOS()
{
    
}

bool veViewerIOS::makeContextCurrent()
{
    veGLView *glView = (__bridge veGLView *)_glView;
    BOOL state = [glView makeContextCurrent];
    return state == YES? true: false;
}

void veViewerIOS::swapBuffers()
{
    veGLView *glView = (__bridge veGLView *)_glView;
    [glView swapBuffers];
}

void veViewerIOS::startRender(veSceneManager *sm)
{
//    veGLView *glView = (__bridge veGLView *)_glView;
//    [glView startRendering];
    if (_isRendering) return;
    _isRendering = true;
    _renderingThread = std::thread([sm, this] {
        while(_isRendering){
            sm->render(this);
        }
    });
}

void veViewerIOS::stopRender(veSceneManager *sm)
{
//    veGLView *glView = (__bridge veGLView *)_glView;
//    [glView stopRendering];
    
    if (!_isRendering) return;
    _isRendering = false;
    _renderingThread.join();
}

void veViewerIOS::create()
{
}

void veViewerIOS::createWithGLView(void *view)
{
    if (_glView) return;
    veGLView *glView = (__bridge veGLView *)view;
    [glView setMultipleTouchEnabled:YES];
    glView.viewer = this;
    glView.app = static_cast<veApplicationIOS *>(veApplication::instance());
    _glView = view;
}

void veViewerIOS::destroy()
{
    if (!_glView) return;
    _glView = nullptr;
}

void veViewerIOS::show()
{
}

void veViewerIOS::hide()
{
}

bool veViewerIOS::isNeedClosed() const
{
    return false;
}

#endif
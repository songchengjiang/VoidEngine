#include "Viewer-android.h"
#include "SceneManager.h"

veViewerAndroid::veViewerAndroid(int width, int height, const std::string &title, veViewerAndroid *sharedViewer)
    : veViewer(width, height, title)
    , _window(nullptr)
    , _sharedViewer(sharedViewer)
    , _isRendering(false)
    , _display(EGL_NO_DISPLAY)
    , _surface(EGL_NO_SURFACE)
    , _context(EGL_NO_CONTEXT)
{
    
}

veViewerAndroid::~veViewerAndroid()
{
    stopRender(nullptr);
    destroy();
    eglDestroyContext(_display, _context);
}

bool veViewerAndroid::makeContextCurrent()
{
    if (_display && _surface && _context){
        if (eglMakeCurrent(_display, _surface, _surface, _context) == EGL_TRUE);
            return true;
    }
    return false;
}

void veViewerAndroid::swapBuffers()
{
    if (_display && _surface)
        eglSwapBuffers(_display, _surface);
}

void veViewerAndroid::startRender(veSceneManager *sm)
{
    if (_isRendering) return;
    _isRendering = true;
    _renderingThread = std::thread([sm, this] {
        while(_isRendering){
            if (_surface && _context)
                sm->render(this);
        }
    });
}

void veViewerAndroid::stopRender(veSceneManager *sm)
{
    if (!_isRendering) return;
    _isRendering = false;
    _renderingThread.join();
}

void veViewerAndroid::create()
{
    if (_display == EGL_NO_DISPLAY){
        _display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (!_display) veLog("_display is null!");
        eglInitialize(_display, nullptr, nullptr);
        EGLint attribs[] = {
                EGL_RED_SIZE,       8,
                EGL_GREEN_SIZE,     8,
                EGL_BLUE_SIZE,      8,
                EGL_ALPHA_SIZE,     8,
                EGL_DEPTH_SIZE,     24,
                EGL_STENCIL_SIZE,   8,
                //EGL_SAMPLE_BUFFERS, 0,
                //EGL_LEVEL,            0,
                EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_NONE
        };

        EGLint format;
        EGLint numConfigs;
        eglChooseConfig(_display, attribs, &_config, 1, &numConfigs);
        if (!numConfigs){
                    veLog("eglChooseConfig failed!");
            return;
        }

        eglGetConfigAttrib(_display, _config, EGL_NATIVE_VISUAL_ID, &format);
        ANativeWindow_setBuffersGeometry(_window, 0, 0, format);
        if (_display){
           veLog("init display");
        }
    }

    if (_surface == EGL_NO_SURFACE){
        _surface = eglCreateWindowSurface(_display, _config, _window, NULL);
        eglQuerySurface( _display, _surface, EGL_WIDTH, &_width );
        eglQuerySurface( _display, _surface, EGL_HEIGHT, &_height );
        if (_surface){
            veLog("init surface");
        }
    }

    if (_context == EGL_NO_CONTEXT){
        const EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, //Request opengl ES3.0
                                           EGL_NONE };
        _context = eglCreateContext(_display, _config, NULL, context_attribs);
        if (_context){
            veLog("init context");
        }
    }
}

void veViewerAndroid::destroy()
{
    //if (_context != EGL_NO_CONTEXT)
    //    eglDestroyContext(_display, _context);
    if (_surface != EGL_NO_SURFACE)
        eglDestroySurface(_display, _surface);
    //_context = EGL_NO_CONTEXT;
    _surface = EGL_NO_SURFACE;
}

void veViewerAndroid::show()
{
}

void veViewerAndroid::hide()
{

}

#ifndef _VE_VIEWER_DESKTOP_
#define _VE_VIEWER_DESKTOP_
#include "Viewer.h"
#include <thread>
#include <mutex>
#include <condition_variable>

#if (VE_PLATFORM == VE_PLATFORM_WIN32) || (VE_PLATFORM == VE_PLATFORM_MAC)

class veSceneManager;
class VE_EXPORT veViewerDesktop : public veViewer
{
    friend class veApplicationDesktop;
public:
    virtual ~veViewerDesktop();
    
    virtual bool makeContextCurrent() override;
    virtual void swapBuffers() override;
    virtual void create() override;
    virtual void destroy() override;
    virtual void show() override;
    virtual void hide() override;
    
    virtual bool simulation(double deltaTime) override;
    virtual void startRender() override;
    virtual void stopRender() override;

protected:
    
    veViewerDesktop(int width, int height, const std::string &title, veViewerDesktop *sharedViewer = nullptr);
    
    
protected:
    
    static void caculateMouseUnitCoords(GLFWwindow* window, double x, double y);
    static void collectKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void collectCharEvent(GLFWwindow* window, unsigned int c);
    static void collectMouseEvent(GLFWwindow* window, int button, int action, int mods);
    static void collectMouseMoveEvent(GLFWwindow* window, double x, double y);
    static void collectScrollEvent(GLFWwindow* window, double x, double y);
    static void collectWindowSizeEvent(GLFWwindow* window, int width, int height);
    static void collectWindowFocusEvent(GLFWwindow* window, int focused);
    static void collectWindowClose(GLFWwindow* window);
    
protected:
    
    GLFWwindow *_hwnd;
    veViewerDesktop *_sharedViewer;
#if VE_PLATFORM == VE_PLATFORM_WIN32
    GLEWContext *_glContext;
#endif
    bool        _isInited;
    
    std::thread _renderingThread;
    bool        _isRendering;
};

#endif

#endif

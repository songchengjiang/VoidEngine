#ifndef _VE_APPLICATION_DESKTOP_
#define _VE_APPLICATION_DESKTOP_
#include "Application.h"
#if (VE_PLATFORM == VE_PLATFORM_WIN32) || (VE_PLATFORM == VE_PLATFORM_MAC)

class VE_EXPORT veApplicationDesktop : public veApplication
{
    friend class veApplication;
	friend class veViewerDesktop;
public:
	virtual ~veApplicationDesktop();

    virtual veViewer* createViewer(int width, int height, const std::string &title, veViewer *sharedContextViewer) override;
    virtual bool run() override;
    virtual void stop() override;

private:

	veApplicationDesktop();

    void updateViewers();
    veViewer* findViewerBy(GLFWwindow* window);

	static void caculateMouseUnitCoords(GLFWwindow* window, double x, double y);
	static void collectKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void collectCharEvent(GLFWwindow* window, unsigned int c);
	static void collectMouseEvent(GLFWwindow* window, int button, int action, int mods);
	static void collectMouseMoveEvent(GLFWwindow* window, double x, double y);
	static void collectScrollEvent(GLFWwindow* window, double x, double y);
	static void collectWindowSizeEvent(GLFWwindow* window, int width, int height);
	static void collectWindowFocusEvent(GLFWwindow* window, int focused);
	static void collectWindowClose(GLFWwindow* window);
    
private:
    
private:
    
    veEvent     _currentEvent;
    bool        _isRunning;
};
#endif

#endif

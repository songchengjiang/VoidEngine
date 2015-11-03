#ifndef _VE_APPLICATION_DESKTOP_
#define _VE_APPLICATION_DESKTOP_
#include "Application.h"

class VE_EXPORT veApplicationDesktop : public veApplication
{
	friend class veApplication;
public:
	virtual ~veApplicationDesktop();

	virtual void makeContextCurrent() override;
	virtual void swapBuffers() override;

private:

	veApplicationDesktop();

	void registerCallbacks();
	void unRegisterCallbacks();

	virtual void dispatchEvents() override;
	virtual void initWindowImplementation(void *param) override;
	virtual bool isWindowShouldClose() override;

	static void caculateMouseUnitCoords(GLFWwindow* window, double x, double y);
	static void collectKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void collectMouseEvent(GLFWwindow* window, int button, int action, int mods);
	static void collectMouseMoveEvent(GLFWwindow* window, double x, double y);
	static void collectScrollEvent(GLFWwindow* window, double x, double y);
	static void collectWindowSizeEvent(GLFWwindow* window, int width, int height);
	static void collectWindowFocusEvent(GLFWwindow* window, int focused);
	static void collectWindowClose(GLFWwindow* window);

private:

	GLFWwindow *_hwnd;
	bool        _isInited;
};

#endif

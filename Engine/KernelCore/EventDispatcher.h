#ifndef _VE_EVENT_DISPATCHER_
#define _VE_EVENT_DISPATCHER_
#include <unordered_map>
#include "Prerequisites.h"
#include "Event.h"

class veVisualiser;
class veNode;
class veEventDispatcher
{
public:

	~veEventDispatcher();

	static veEventDispatcher* instance();

	void registerCallback();

	bool dispatch(double deltaTime);


protected:

	veEventDispatcher();

private:

	static void caculateMouseUnitCoords(GLFWwindow* window, double x, double y);

	static void collectKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
	//static void collectCharEvent(GLFWwindow* window, unsigned int codepoint, int mods);
	static void collectMouseEvent(GLFWwindow* window, int button, int action, int mods);
	static void collectMouseMoveEvent(GLFWwindow* window, double x, double y);
	static void collectScrollEvent(GLFWwindow* window, double x, double y);
	static void collectWindowSizeEvent(GLFWwindow* window, int width, int height);
	static void collectWindowFocusEvent(GLFWwindow* window, int focused);
	static void collectWindowClose(GLFWwindow* window);

private:

	typedef std::unordered_map< GLFWwindow*, std::vector<veEvent> > Events;
	Events _events;
	veEvent _currentEvent;
};

#endif
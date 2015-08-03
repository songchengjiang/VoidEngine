#ifndef _VE_EVENT_DISPATCHER_
#define _VE_EVENT_DISPATCHER_
#include <unordered_map>
#include "Prerequisites.h"
#include "Event.h"

class veVisualiser;
class VE_EXPORT veEventDispatcher
{
public:

	~veEventDispatcher();

	static veEventDispatcher* instance();

	void dispatch();


protected:

	veEventDispatcher();
	void initEventCallbacks();

private:

	static void caculateMouseUnitCoords(veVisualiser *vs, double x, double y);

	static void collectKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
	//static void collectCharEvent(GLFWwindow* window, unsigned int codepoint, int mods);
	static void collectMouseEvent(GLFWwindow* window, int button, int action, int mods);
	static void collectMouseMoveEvent(GLFWwindow* window, double x, double y);
	static void collectScrollEvent(GLFWwindow* window, double x, double y);
	static void collectWindowSizeEvent(GLFWwindow* window, int width, int height);
	static void collectWindowFocusEvent(GLFWwindow* window, int focused);

private:

	typedef std::unordered_map< GLFWwindow*, std::vector<veEvent> > Events;
	Events _events;
	veEvent _currentEvent;
};

#endif
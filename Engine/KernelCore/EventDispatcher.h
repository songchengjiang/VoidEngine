#ifndef _VE_EVENT_DISPATCHER_
#define _VE_EVENT_DISPATCHER_
#include <unordered_map>
#include "BaseCore/Vector2.h"
#include "Prerequisites.h"
#include "Event.h"
#include "Registrar.h"

class veVisualiser;
class VE_EXPORT veEventDispatcher
{
public:
	typedef std::unordered_map< veVisualiser*, std::vector<veEvent> > Events;

	~veEventDispatcher();

	static veEventDispatcher* instance();


protected:

	veEventDispatcher();
	void initEventCallbacks();

private:

	void caculateMouseUnitCoords(veVisualiser *vs, double x, double y);

	static void collectKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void collectCharEvent(GLFWwindow* window, unsigned int codepoint);
	static void collectMouseEvent(GLFWwindow* window, int button, int action, int mods);
	static void collectMouseMoveEvent(GLFWwindow* window, double x, double y);
	static void collectScrollEvent(GLFWwindow* window, double x, double y);
	static void collectWindowSizeEvent(GLFWwindow* window, int width, int height);
	static void collectWindowFocusEvent(GLFWwindow* window, int focused);

private:

	Events _events;
	veVec2 _mouseUnitCoords;
};

#endif
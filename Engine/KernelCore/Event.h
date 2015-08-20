#ifndef _VE_EVENT_
#define _VE_EVENT_
#include "Prerequisites.h"

class VE_EXPORT veEvent
{
public:

	veEvent()
	: _eventType(VE_UNKNOWN)
	, _keySymbol(VE_KEY_UNKNOWN)
	, _mouseSymbol(VE_MOUSE_UNKNOWN)
	, _mouseX(0.0f)
	, _mouseY(0.0f)
	, _wndWidth(0)
	, _wndHeight(0){
	}

	~veEvent(){

	}

	enum EventType{
		VE_UNKNOWN     = -1,
		VE_RELEASE     = 0,
		VE_PRESS       = 1 << 1,
		VE_DOWN        = 1 << 2,
		VE_UP          = 1 << 3,
		VE_REPEAT      = 1 << 4,
		VE_SCROLL_DOWN = 1 << 5,
		VE_SCROLL_UP   = 1 << 6,
		VE_MOVE        = 1 << 7,
		VE_WIN_RESIZE  = 1 << 8,
		VE_WIN_FOCUS   = 1 << 9,
		VE_WIN_CLOSE   = 1 << 10,
		VE_KEYBOARD_EVENT = VE_DOWN | VE_UP | VE_REPEAT,
		VE_MOUSE_EVENT = VE_RELEASE | VE_PRESS | VE_SCROLL_DOWN | VE_SCROLL_UP | VE_MOVE,
		VE_WIN_EVENT   = VE_WIN_RESIZE | VE_WIN_FOCUS | VE_WIN_CLOSE,
		VE_ALL_EVENT   = 0xFFFF,
	};

	enum KeySymbol{
		VE_KEY_UNKNOWN       = GLFW_KEY_UNKNOWN      ,

		/* Printable keys */
		VE_KEY_SPACE         = GLFW_KEY_SPACE        ,
		VE_KEY_APOSTROPHE    = GLFW_KEY_APOSTROPHE   ,
		VE_KEY_COMMA         = GLFW_KEY_COMMA        ,
		VE_KEY_MINUS         = GLFW_KEY_MINUS        ,
		VE_KEY_PERIOD        = GLFW_KEY_PERIOD       ,
		VE_KEY_SLASH         = GLFW_KEY_SLASH        ,
		VE_KEY_0             = GLFW_KEY_0            ,
		VE_KEY_1             = GLFW_KEY_1            ,
		VE_KEY_2             = GLFW_KEY_2            ,
		VE_KEY_3             = GLFW_KEY_3            ,
		VE_KEY_4             = GLFW_KEY_4            ,
		VE_KEY_5             = GLFW_KEY_5            ,
		VE_KEY_6             = GLFW_KEY_6            ,
		VE_KEY_7             = GLFW_KEY_7            ,
		VE_KEY_8             = GLFW_KEY_8            ,
		VE_KEY_9             = GLFW_KEY_9            ,
		VE_KEY_SEMICOLON     = GLFW_KEY_SEMICOLON    ,
		VE_KEY_EQUAL         = GLFW_KEY_EQUAL        ,
		VE_KEY_A             = GLFW_KEY_A            ,
		VE_KEY_B             = GLFW_KEY_B            ,
		VE_KEY_C             = GLFW_KEY_C            ,
		VE_KEY_D             = GLFW_KEY_D            ,
		VE_KEY_E             = GLFW_KEY_E            ,
		VE_KEY_F             = GLFW_KEY_F            ,
		VE_KEY_G             = GLFW_KEY_G            ,
		VE_KEY_H             = GLFW_KEY_H            ,
		VE_KEY_I             = GLFW_KEY_I            ,
		VE_KEY_J             = GLFW_KEY_J            ,
		VE_KEY_K             = GLFW_KEY_K            ,
		VE_KEY_L             = GLFW_KEY_L            ,
		VE_KEY_M             = GLFW_KEY_M            ,
		VE_KEY_N             = GLFW_KEY_N            ,
		VE_KEY_O             = GLFW_KEY_O            ,
		VE_KEY_P             = GLFW_KEY_P            ,
		VE_KEY_Q             = GLFW_KEY_Q            ,
		VE_KEY_R             = GLFW_KEY_R            ,
		VE_KEY_S             = GLFW_KEY_S            ,
		VE_KEY_T             = GLFW_KEY_T            ,
		VE_KEY_U             = GLFW_KEY_U            ,
		VE_KEY_V             = GLFW_KEY_V            ,
		VE_KEY_W             = GLFW_KEY_W            ,
		VE_KEY_X             = GLFW_KEY_X            ,
		VE_KEY_Y             = GLFW_KEY_Y            ,
		VE_KEY_Z             = GLFW_KEY_Z            ,
		VE_KEY_LEFT_BRACKET  = GLFW_KEY_LEFT_BRACKET ,
		VE_KEY_BACKSLASH     = GLFW_KEY_BACKSLASH    ,
		VE_KEY_RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET,
		VE_KEY_GRAVE_ACCENT  = GLFW_KEY_GRAVE_ACCENT ,
		VE_KEY_WORLD_1       = GLFW_KEY_WORLD_1      ,
		VE_KEY_WORLD_2       = GLFW_KEY_WORLD_2      ,

		/* Function keys */
		VE_KEY_ESCAPE        = GLFW_KEY_ESCAPE       ,
		VE_KEY_ENTER         = GLFW_KEY_ENTER        ,
		VE_KEY_TAB           = GLFW_KEY_TAB          ,
		VE_KEY_BACKSPACE     = GLFW_KEY_BACKSPACE    ,
		VE_KEY_INSERT        = GLFW_KEY_INSERT       ,
		VE_KEY_DELETE        = GLFW_KEY_DELETE       ,
		VE_KEY_RIGHT         = GLFW_KEY_RIGHT        ,
		VE_KEY_LEFT          = GLFW_KEY_LEFT         ,
		VE_KEY_DOWN          = GLFW_KEY_DOWN         ,
		VE_KEY_UP            = GLFW_KEY_UP           ,
		VE_KEY_PAGE_UP       = GLFW_KEY_PAGE_UP      ,
		VE_KEY_PAGE_DOWN     = GLFW_KEY_PAGE_DOWN    ,
		VE_KEY_HOME          = GLFW_KEY_HOME         ,
		VE_KEY_END           = GLFW_KEY_END          ,
		VE_KEY_CAPS_LOCK     = GLFW_KEY_CAPS_LOCK    ,
		VE_KEY_SCROLL_LOCK   = GLFW_KEY_SCROLL_LOCK  ,
		VE_KEY_NUM_LOCK      = GLFW_KEY_NUM_LOCK     ,
		VE_KEY_PRINT_SCREEN  = GLFW_KEY_PRINT_SCREEN ,
		VE_KEY_PAUSE         = GLFW_KEY_PAUSE        ,
		VE_KEY_F1            = GLFW_KEY_F1           ,
		VE_KEY_F2            = GLFW_KEY_F2           ,
		VE_KEY_F3            = GLFW_KEY_F3           ,
		VE_KEY_F4            = GLFW_KEY_F4           ,
		VE_KEY_F5            = GLFW_KEY_F5           ,
		VE_KEY_F6            = GLFW_KEY_F6           ,
		VE_KEY_F7            = GLFW_KEY_F7           ,
		VE_KEY_F8            = GLFW_KEY_F8           ,
		VE_KEY_F9            = GLFW_KEY_F9           ,
		VE_KEY_F10           = GLFW_KEY_F10          ,
		VE_KEY_F11           = GLFW_KEY_F11          ,
		VE_KEY_F12           = GLFW_KEY_F12          ,
		VE_KEY_F13           = GLFW_KEY_F13          ,
		VE_KEY_F14           = GLFW_KEY_F14          ,
		VE_KEY_F15           = GLFW_KEY_F15          ,
		VE_KEY_F16           = GLFW_KEY_F16          ,
		VE_KEY_F17           = GLFW_KEY_F17          ,
		VE_KEY_F18           = GLFW_KEY_F18          ,
		VE_KEY_F19           = GLFW_KEY_F19          ,
		VE_KEY_F20           = GLFW_KEY_F20          ,
		VE_KEY_F21           = GLFW_KEY_F21          ,
		VE_KEY_F22           = GLFW_KEY_F22          ,
		VE_KEY_F23           = GLFW_KEY_F23          ,
		VE_KEY_F24           = GLFW_KEY_F24          ,
		VE_KEY_F25           = GLFW_KEY_F25          ,
		VE_KEY_KP_0          = GLFW_KEY_KP_0         ,
		VE_KEY_KP_1          = GLFW_KEY_KP_1         ,
		VE_KEY_KP_2          = GLFW_KEY_KP_2         ,
		VE_KEY_KP_3          = GLFW_KEY_KP_3         ,
		VE_KEY_KP_4          = GLFW_KEY_KP_4         ,
		VE_KEY_KP_5          = GLFW_KEY_KP_5         ,
		VE_KEY_KP_6          = GLFW_KEY_KP_6         ,
		VE_KEY_KP_7          = GLFW_KEY_KP_7         ,
		VE_KEY_KP_8          = GLFW_KEY_KP_8         ,
		VE_KEY_KP_9          = GLFW_KEY_KP_9         ,
		VE_KEY_KP_DECIMAL    = GLFW_KEY_KP_DECIMAL   ,
		VE_KEY_KP_DIVIDE     = GLFW_KEY_KP_DIVIDE    ,
		VE_KEY_KP_MULTIPLY   = GLFW_KEY_KP_MULTIPLY  ,
		VE_KEY_KP_SUBTRACT   = GLFW_KEY_KP_SUBTRACT  ,
		VE_KEY_KP_ADD        = GLFW_KEY_KP_ADD       ,
		VE_KEY_KP_ENTER      = GLFW_KEY_KP_ENTER     ,
		VE_KEY_KP_EQUAL      = GLFW_KEY_KP_EQUAL     ,
		VE_KEY_LEFT_SHIFT    = GLFW_KEY_LEFT_SHIFT   ,
		VE_KEY_LEFT_CONTROL  = GLFW_KEY_LEFT_CONTROL ,
		VE_KEY_LEFT_ALT      = GLFW_KEY_LEFT_ALT     ,
		VE_KEY_LEFT_SUPER    = GLFW_KEY_LEFT_SUPER   ,
		VE_KEY_RIGHT_SHIFT   = GLFW_KEY_RIGHT_SHIFT  ,
		VE_KEY_RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL,
		VE_KEY_RIGHT_ALT     = GLFW_KEY_RIGHT_ALT    ,
		VE_KEY_RIGHT_SUPER   = GLFW_KEY_RIGHT_SUPER  ,
		VE_KEY_MENU          = GLFW_KEY_MENU         ,
	};

	enum ModKeySymbol{
		VE_MOD_SHIFT   = GLFW_MOD_SHIFT,
		VE_MOD_CONTROL = GLFW_MOD_CONTROL,
		VE_MOD_ALT     = GLFW_MOD_ALT,
		VE_MOD_SUPER   = GLFW_MOD_SUPER,
	};

	enum MouseSymbol{
		VE_MOUSE_UNKNOWN       = -1,
		VE_MOUSE_BUTTON_1      = GLFW_MOUSE_BUTTON_1     ,
		VE_MOUSE_BUTTON_2      = GLFW_MOUSE_BUTTON_2     ,
		VE_MOUSE_BUTTON_3      = GLFW_MOUSE_BUTTON_3     ,
		VE_MOUSE_BUTTON_4      = GLFW_MOUSE_BUTTON_4     ,
		VE_MOUSE_BUTTON_5      = GLFW_MOUSE_BUTTON_5     ,
		VE_MOUSE_BUTTON_6      = GLFW_MOUSE_BUTTON_6     ,
		VE_MOUSE_BUTTON_7      = GLFW_MOUSE_BUTTON_7     ,
		VE_MOUSE_BUTTON_8      = GLFW_MOUSE_BUTTON_8     ,
		VE_MOUSE_BUTTON_LAST   = GLFW_MOUSE_BUTTON_LAST  ,
		VE_MOUSE_BUTTON_LEFT   = GLFW_MOUSE_BUTTON_LEFT  ,
		VE_MOUSE_BUTTON_RIGHT  = GLFW_MOUSE_BUTTON_RIGHT ,
		VE_MOUSE_BUTTON_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
	};

	void setEventType(const EventType et) { _eventType = et; }
	EventType getEventType() const { return _eventType; }
	void setKeySymbol(const int ks) { _keySymbol = ks; }
	int getKeySymbol() const { return _keySymbol; }
	void setModKeySymbol(const ModKeySymbol mks) { _modKeySymbol = mks; }
	ModKeySymbol getModKeySymbol() const { return _modKeySymbol; }
	void setMouseSymbol(const MouseSymbol ms) { _mouseSymbol = ms; }
	MouseSymbol getMouseSymbol() const { return _mouseSymbol; }

	void setMouseX(veReal x) { _mouseX = x; }
	veReal getMouseX() const { return _mouseX; }
	void setMouseY(veReal y) { _mouseY = y; }
	veReal getMouseY() const { return _mouseY; }

	void setWindowWidth(int width) { _wndWidth = width; }
	int getWindowWidth() const { return _wndWidth; }
	void setWindowHeight(int height) { _wndHeight = height; }
	int getWindowHeight() const { return _wndHeight; }

private:

	EventType _eventType;
	int _keySymbol;
	ModKeySymbol _modKeySymbol;
	MouseSymbol _mouseSymbol;

	veReal _mouseX;
	veReal _mouseY;
	int _wndWidth;
	int _wndHeight;
};

#endif
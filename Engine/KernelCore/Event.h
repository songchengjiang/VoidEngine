#ifndef _VE_EVENT_
#define _VE_EVENT_

#include <unordered_map>
#include "Prerequisites.h"

class VE_EXPORT veEvent
{
public:

	veEvent()
	: _eventType(VE_UNKNOWN)
	, _keySymbol(VE_KEY_UNKNOWN)
	, _mouseSymbol(VE_MOUSE_UNKNOWN)
    , _keyChar(0)
	, _mouseX(0.0f)
	, _mouseY(0.0f)
    , _latestMouseX(0.0f)
    , _latestMouseY(0.0f)
    , _mouseScroll(0.0f)
	, _wndWidth(0)
	, _wndHeight(0){
	}

	~veEvent(){

	}

	enum EventType{
		VE_UNKNOWN     = -1,
		VE_RELEASE     = 1,
		VE_PRESS       = 1 << 1,
		VE_DOWN        = 1 << 2,
		VE_UP          = 1 << 3,
		VE_REPEAT      = 1 << 4,
		VE_SCROLL_DOWN = 1 << 5,
		VE_SCROLL_UP   = 1 << 6,
		VE_MOVE        = 1 << 7,
		VE_DRAG        = 1 << 8,
		VE_TOUCH_START = 1 << 9,
		VE_TOUCH_MOVE  = 1 << 10,
		VE_TOUCH_END   = 1 << 11,
        VE_WIN_INIT    = 1 << 12,
		VE_WIN_RESIZE  = 1 << 13,
		VE_WIN_FOCUS   = 1 << 14,
		VE_WIN_NOFOCUS = 1 << 15,
		VE_WIN_CLOSE   = 1 << 16,
		VE_KEYBOARD_EVENT = VE_DOWN | VE_UP | VE_REPEAT,
		VE_MOUSE_EVENT = VE_RELEASE | VE_PRESS | VE_SCROLL_DOWN | VE_SCROLL_UP | VE_MOVE | VE_DRAG,
		VE_TOUCH_EVENT = VE_TOUCH_START | VE_TOUCH_MOVE | VE_TOUCH_END,
		VE_WIN_EVENT   = VE_WIN_INIT | VE_WIN_RESIZE | VE_WIN_FOCUS | VE_WIN_NOFOCUS |VE_WIN_CLOSE,
		VE_ALL_EVENT   = 0xFFFF,
	};

	enum KeySymbol{
		VE_KEY_UNKNOWN,

		/* Printable keys */
		VE_KEY_SPACE,
		VE_KEY_APOSTROPHE,
		VE_KEY_COMMA,
		VE_KEY_MINUS,
		VE_KEY_PERIOD,
		VE_KEY_SLASH,
		VE_KEY_0,
		VE_KEY_1,
		VE_KEY_2,
		VE_KEY_3,
		VE_KEY_4,
		VE_KEY_5,
		VE_KEY_6,
		VE_KEY_7,
		VE_KEY_8,
		VE_KEY_9,
		VE_KEY_SEMICOLON,
		VE_KEY_EQUAL,
		VE_KEY_A,
		VE_KEY_B,
		VE_KEY_C,
		VE_KEY_D,
		VE_KEY_E,
		VE_KEY_F,
		VE_KEY_G,
		VE_KEY_H,
		VE_KEY_I,
		VE_KEY_J,
		VE_KEY_K,
		VE_KEY_L,
		VE_KEY_M,
		VE_KEY_N,
		VE_KEY_O,
		VE_KEY_P,
		VE_KEY_Q,
		VE_KEY_R,
		VE_KEY_S,
		VE_KEY_T,
		VE_KEY_U,
		VE_KEY_V,
		VE_KEY_W,
		VE_KEY_X,
		VE_KEY_Y,
		VE_KEY_Z,
		VE_KEY_LEFT_BRACKET,
		VE_KEY_BACKSLASH,
		VE_KEY_RIGHT_BRACKET,
		VE_KEY_GRAVE_ACCENT,
		VE_KEY_WORLD_1,
		VE_KEY_WORLD_2,

		/* Function keys */
		VE_KEY_ESCAPE,
		VE_KEY_ENTER,
		VE_KEY_TAB,
		VE_KEY_BACKSPACE,
		VE_KEY_INSERT,
		VE_KEY_DELETE,
		VE_KEY_RIGHT,
		VE_KEY_LEFT,
		VE_KEY_DOWN,
		VE_KEY_UP,
		VE_KEY_PAGE_UP,
		VE_KEY_PAGE_DOWN,
		VE_KEY_HOME,
		VE_KEY_END,
		VE_KEY_CAPS_LOCK,
		VE_KEY_SCROLL_LOCK,
		VE_KEY_NUM_LOCK,
		VE_KEY_PRINT_SCREEN,
		VE_KEY_PAUSE,
		VE_KEY_F1,
		VE_KEY_F2,
		VE_KEY_F3,
		VE_KEY_F4,
		VE_KEY_F5,
		VE_KEY_F6,
		VE_KEY_F7,
		VE_KEY_F8,
		VE_KEY_F9,
		VE_KEY_F10,
		VE_KEY_F11,
		VE_KEY_F12,
		VE_KEY_F13,
		VE_KEY_F14,
		VE_KEY_F15,
		VE_KEY_F16,
		VE_KEY_F17,
		VE_KEY_F18,
		VE_KEY_F19,
		VE_KEY_F20,
		VE_KEY_F21,
		VE_KEY_F22,
		VE_KEY_F23,
		VE_KEY_F24,
		VE_KEY_F25,
		VE_KEY_KP_0,
		VE_KEY_KP_1,
		VE_KEY_KP_2,
		VE_KEY_KP_3,
		VE_KEY_KP_4,
		VE_KEY_KP_5,
		VE_KEY_KP_6,
		VE_KEY_KP_7,
		VE_KEY_KP_8,
		VE_KEY_KP_9,
		VE_KEY_KP_DECIMAL,
		VE_KEY_KP_DIVIDE,
		VE_KEY_KP_MULTIPLY,
		VE_KEY_KP_SUBTRACT,
		VE_KEY_KP_ADD,
		VE_KEY_KP_ENTER,
		VE_KEY_KP_EQUAL,
		VE_KEY_LEFT_SHIFT,
		VE_KEY_LEFT_CONTROL,
		VE_KEY_LEFT_ALT,
		VE_KEY_LEFT_SUPER,
		VE_KEY_RIGHT_SHIFT,
		VE_KEY_RIGHT_CONTROL,
		VE_KEY_RIGHT_ALT,
		VE_KEY_RIGHT_SUPER,
		VE_KEY_MENU,
	};

	enum ModKeySymbol{
		VE_MOD_SHIFT,
		VE_MOD_CONTROL,
		VE_MOD_ALT,
		VE_MOD_SUPER,
	};

	enum MouseSymbol{
		VE_MOUSE_UNKNOWN,
		VE_MOUSE_BUTTON_1,
		VE_MOUSE_BUTTON_2,
		VE_MOUSE_BUTTON_3,
		VE_MOUSE_BUTTON_4,
		VE_MOUSE_BUTTON_5,
		VE_MOUSE_BUTTON_6,
		VE_MOUSE_BUTTON_7,
		VE_MOUSE_BUTTON_8,
		VE_MOUSE_BUTTON_LAST,
		VE_MOUSE_BUTTON_LEFT,
		VE_MOUSE_BUTTON_RIGHT,
		VE_MOUSE_BUTTON_MIDDLE,
	};

	struct Touch{
		int    id;
		veReal latestx;
		veReal latesty;
		veReal x;
		veReal y;
	};
	typedef std::vector<Touch> Touchs;

	void setEventType(const EventType et) { _eventType = et; }
	EventType getEventType() const { return _eventType; }
	void setKeySymbol(const KeySymbol ks) { _keySymbol = ks; }
	KeySymbol getKeySymbol() const { return _keySymbol; }
    void setKeyChar(unsigned int kc) { _keyChar = kc; }
    unsigned int getKeyChar() const { return _keyChar; }
	void setModKeySymbol(const ModKeySymbol mks) { _modKeySymbol = mks; }
	ModKeySymbol getModKeySymbol() const { return _modKeySymbol; }
	void setMouseSymbol(const MouseSymbol ms) { _mouseSymbol = ms; }
	MouseSymbol getMouseSymbol() const { return _mouseSymbol; }

    void setMouseX(veReal x) { _latestMouseX = _mouseX; _mouseX = x; }
	veReal getMouseX() const { return _mouseX; }
    void setMouseY(veReal y) { _latestMouseY = _mouseY; _mouseY = y; }
	veReal getMouseY() const { return _mouseY; }
    veReal getLatestMouseX() const { return _latestMouseX; }
    veReal getLatestMouseY() const { return _latestMouseY; }
    void setMouseScroll(veReal val) { _mouseScroll = val; }
    veReal getMouseScroll() const { return _mouseScroll; }

	void addTouch(const Touch &touch) { _touches.push_back(touch); }
	void removeTouch(int id) {
		for (auto iter = _touches.begin(); iter != _touches.end(); ++iter) {
			if (iter->id == id) {
				_touches.erase(iter);
				return;
			}
		}
	}

	Touchs& getTouches() { return _touches; }
	const Touchs& getTouches() const { return _touches;}

	void setWindowWidth(int width) { _wndWidth = width; }
	int getWindowWidth() const { return _wndWidth; }
	void setWindowHeight(int height) { _wndHeight = height; }
	int getWindowHeight() const { return _wndHeight; }

private:

	EventType _eventType;
	KeySymbol _keySymbol;
	ModKeySymbol _modKeySymbol;
	MouseSymbol _mouseSymbol;
	Touchs    _touches;

    unsigned int _keyChar;
	veReal _mouseX;
	veReal _mouseY;
    veReal _latestMouseX;
    veReal _latestMouseY;
    veReal _mouseScroll;
	int _wndWidth;
	int _wndHeight;
};

typedef std::vector<veEvent> veEventList;

#endif
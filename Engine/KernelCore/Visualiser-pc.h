#ifndef _VE_VISUALISER_PC_
#define _VE_VISUALISER_PC_
#include "Visualiser.h"

class VE_EXPORT veVisualiserPC : public veVisualiser
{
	friend class veSceneManager;
public:
	~veVisualiserPC();

	//int addCamera(veCamera *camera);
	//veCamera* getCamera(unsigned int idx);
	//veCamera* removeCamera(unsigned int idx);
	//unsigned int getCameraNum() const { return _cameras.size(); }

	virtual bool dispatchEvents() override;

protected:

	veVisualiserPC(int w, int h, const std::string &title);
	virtual void setContextCurrent() override;
	virtual void swapBuffers() override;
	virtual bool isWindowShouldClose() override;

private:

	void registerCallbacks();
	void unRegisterCallbacks();
	static void caculateMouseUnitCoords(GLFWwindow* window, double x, double y);
	static void collectKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void collectMouseEvent(GLFWwindow* window, int button, int action, int mods);
	static void collectMouseMoveEvent(GLFWwindow* window, double x, double y);
	static void collectScrollEvent(GLFWwindow* window, double x, double y);
	static void collectWindowSizeEvent(GLFWwindow* window, int width, int height);
	static void collectWindowFocusEvent(GLFWwindow* window, int focused);
	static void collectWindowClose(GLFWwindow* window);

protected:

	int _width;
	int _height;
	std::string _title;
	bool        _isInited;
	GLFWwindow *_hwnd;
	VE_Ptr<veCamera> _mainCamera;

	veSceneManager *_sceneManager;
};

typedef veRegistrar<GLFWwindow*, veVisualiserPC> veVisualiserRegistrar;

#endif
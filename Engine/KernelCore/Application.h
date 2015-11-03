#ifndef _VE_APPLICATION_
#define _VE_APPLICATION_
#include "Prerequisites.h"
#include "VE_Ptr.h"
#include "SceneManager.h"
#include "Event.h"

class veSceneManager;
class VE_EXPORT veApplication
{
public:
	virtual ~veApplication();

	static veApplication* instance();

	void initWindow(int w, int h, const std::string &title, void *param);
	int width() { return _width; }
	int height() { return _height; }

	void setSceneManager(veSceneManager *sm);
	veSceneManager* getSceneManager() { return _sceneManager.get(); }

	virtual void makeContextCurrent() = 0;
	virtual void swapBuffers() = 0;

	bool run();
	void stop();

protected:

	veApplication();

	virtual void dispatchEvents() = 0;
	virtual void initWindowImplementation(void *param) = 0;
	virtual bool isWindowShouldClose() = 0;

protected:
	
	//typedef std::vector< VE_Ptr<veVisualiser> > VisualiserList;
	//VisualiserList _visualiserList;
	VE_Ptr<veSceneManager> _sceneManager;
	bool                   _isRunning;

	int _width;
	int _height;
	std::string _title;
	Events  _events;
	veEvent _currentEvent;
};

#endif

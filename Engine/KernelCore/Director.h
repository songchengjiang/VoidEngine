#ifndef _VE_DIRECTOR_
#define _VE_DIRECTOR_
#include "Prerequisites.h"
#include "VE_Ptr.h"
#include "SceneManager.h"

class veSceneManager;
class VE_EXPORT veDirector
{
public:
	~veDirector();

	static veDirector* instance();

	void setSceneManager(veSceneManager *sm);
	veSceneManager* getSceneManager() { return _sceneManager.get(); }

	bool run();
	void stop();
	void startThreading();
	void stopThreading();
	bool frame(double deltaTime);

private:

	veDirector();

private:
	
	//typedef std::vector< VE_Ptr<veVisualiser> > VisualiserList;
	//VisualiserList _visualiserList;
	VE_Ptr<veSceneManager> _sceneManager;
	bool                   _isRunning;
};

#endif

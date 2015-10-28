#ifndef _VE_DIRECTOR_
#define _VE_DIRECTOR_
#include "Prerequisites.h"
#include "VE_Ptr.h"
#include "Visualiser.h"
#include "SceneManager.h"

class veSceneManager;
class VE_EXPORT veDirector
{
public:
	~veDirector();

	static veDirector* instance();

	//veVisualiser* createVisualiser(int w, int h, const std::string &title);
	void setSceneManager(veSceneManager *sm);
	veSceneManager* getSceneManager() { return _sceneManager.get(); }

	bool run();
	void stop();

private:

	veDirector();

private:
	
	//typedef std::vector< VE_Ptr<veVisualiser> > VisualiserList;
	//VisualiserList _visualiserList;
	VE_Ptr<veSceneManager> _sceneManager;
	bool           _isRunning;
};

#endif

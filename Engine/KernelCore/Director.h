#ifndef _VE_DIRECTOR_
#define _VE_DIRECTOR_
#include "Prerequisites.h"
#include "VE_Ptr.h"
#include "Visualiser.h"

class veSceneManager;
class VE_EXPORT veDirector
{
public:
	~veDirector();

	static veDirector* instance();

	//veVisualiser* createVisualiser(int w, int h, const std::string &title);
	veSceneManager* getSceneManager() { return _sceneManager; }

	bool run();
	void stop();

private:

	veDirector();
	void allocSceneManager();

private:
	
	//typedef std::vector< VE_Ptr<veVisualiser> > VisualiserList;
	//VisualiserList _visualiserList;
	veSceneManager *_sceneManager;
	bool           _isRunning;
};

#endif

#ifndef _VE_APPLICATION_
#define _VE_APPLICATION_
#include "Prerequisites.h"
#include "VE_Ptr.h"
#include "SceneManager.h"
#include "Event.h"

class veViewer;
class VE_EXPORT veApplication
{
public:
	virtual ~veApplication();

	static veApplication* instance();

    virtual veViewer* createViewer(int width, int height, const std::string &title, veViewer *sharedContextViewer = nullptr) = 0;
    veViewer* getViewer(size_t idx);
    size_t getViewerCount() const { return _viewerList.size(); }
	virtual bool run() = 0;
	virtual void stop() = 0;

protected:

	veApplication();
    
protected:
    
    std::vector<veViewer *> _viewerList;
};

#endif

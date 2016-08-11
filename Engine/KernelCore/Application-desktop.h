#ifndef _VE_APPLICATION_DESKTOP_
#define _VE_APPLICATION_DESKTOP_
#include "Application.h"
#if (VE_PLATFORM == VE_PLATFORM_WIN32) || (VE_PLATFORM == VE_PLATFORM_MAC)

class VE_EXPORT veApplicationDesktop : public veApplication
{
    friend class veApplication;
	friend class veViewerDesktop;
public:
	virtual ~veApplicationDesktop();

    virtual veViewer* createViewer(int width, int height, const std::string &title, veViewer *sharedContextViewer) override;
    virtual bool run() override;
    virtual void stop() override;

private:

	veApplicationDesktop();
    
private:
    
    void updateViewers();
    
private:
    
    veEvent     _currentEvent;
    bool        _isRunning;
};
#endif

#endif

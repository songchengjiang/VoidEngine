#ifndef _VE_APPLICATION_IOS_
#define _VE_APPLICATION_IOS_
#include "Application.h"

#if (VE_PLATFORM == VE_PLATFORM_IOS)
class VE_EXPORT veApplicationIOS : public veApplication
{
    friend class veApplication;
public:
    virtual ~veApplicationIOS();

    virtual veViewer* createViewer(int width, int height, const std::string &title, veViewer *sharedContextViewer = nullptr)  override;
    
    virtual bool run() override ;
    virtual void stop() override ;


private:

    veApplicationIOS();

};
#endif

#endif //ANDROID_APPLICATION_ANDROID_H

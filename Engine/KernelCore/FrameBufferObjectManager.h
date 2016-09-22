#ifndef _VE_FRAME_BUFFER_OBJECT_MANAGER_
#define _VE_FRAME_BUFFER_OBJECT_MANAGER_
#include "Prerequisites.h"
#include "BaseManager.h"
#include "FrameBufferObject.h"


class VE_EXPORT veFrameBufferObjectManager : public veBaseManager
{
	friend class veSceneManager;
public:

	~veFrameBufferObjectManager();

	virtual void resourceRecovery(unsigned int contextID) override;

	veFrameBufferObject* findFrameBufferObject(const std::string &name);
	veFrameBufferObject* createFrameBufferObject(const std::string &name);

	static std::string TYPE() { return "FRAME_BUFFER_OBJECT"; }

private:

	veFrameBufferObjectManager(veSceneManager *sm);
    
	std::vector< VE_Ptr<veFrameBufferObject> > _fboPool;
};


#endif
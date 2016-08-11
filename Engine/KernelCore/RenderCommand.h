#ifndef _VE_RENDER_COMMAND_
#define _VE_RENDER_COMMAND_
#include "Prerequisites.h"
#include "MatrixPtr.h"
#include "VE_Ptr.h"
#include "BaseCore/Array.h"
#include <functional>
#include <unordered_map>

class veRenderer;
class veRenderableObject;
class veSceneManager;
class veCamera;
class veNode;
class vePass;
class veLight;
struct veRenderCommand
{
	veRenderCommand()
		: priority(NORMAL_PRIORITY)
		, depthInCamera(0.0f)
		, user1(nullptr)
		, user2(nullptr)
		, user3(nullptr)
        , contextID(0){
	}

	enum Priority
	{
		LOW_PRIORITY    = -1,
		NORMAL_PRIORITY = 0,
		HIGH_PRIORITY   = 1,
	};

	int                 priority;
	unsigned int        mask;
	VE_Ptr<veMat4Ptr>   worldMatrix;
	veReal              depthInCamera;
	vePass             *pass;
	//veNode             *attachedNode;
	//veRenderableObject *renderableObj;
	veCamera           *camera;
	veSceneManager     *sceneManager;
	void*               user1;
	void*               user2;
	void*               user3;
	veRenderer         *renderer;
    unsigned int        contextID;
    //std::function<void(const veRenderCommand &)> drawFunc;
};

#endif

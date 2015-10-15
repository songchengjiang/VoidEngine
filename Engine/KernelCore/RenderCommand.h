#ifndef _VE_RENDER_COMMAND_
#define _VE_RENDER_COMMAND_
#include "Prerequisites.h"
#include "BaseCore/Matrix4.h"
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
		, depthInCamera(0.0f){
	}

	enum Priority
	{
		LOW_PRIORITY    = -1,
		NORMAL_PRIORITY = 0,
		HIGH_PRIORITY   = 1,
	};

	int                 priority;
	veReal              depthInCamera;
	vePass             *pass;
	//veNode             *attachedNode;
	veMat4              worldMatrix;
	veRenderableObject *renderableObj;
	veCamera           *camera;
	veSceneManager     *sceneManager;
	//veRenderer         *renderer;
    std::function<void(const veRenderCommand &)> drawFunc;
};

#endif

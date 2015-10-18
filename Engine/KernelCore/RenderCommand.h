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
	veMat4              worldMatrix;
	veReal              depthInCamera;
	vePass             *pass;
	//veNode             *attachedNode;
	veRenderableObject *renderableObj;
	veCamera           *camera;
	veSceneManager     *sceneManager;
	void               *userData;
	//veRenderer         *renderer;
    std::function<void(const veRenderCommand &)> drawFunc;
};

#endif

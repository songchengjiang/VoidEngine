#include "Visualiser.h"
#include "Node.h"
#include "NodeVisitor.h"
#include "SceneManager.h"

veVisualiser::veVisualiser(int w, int h, const std::string &title)
	: USE_VE_PTR_INIT
	, _width(w)
	, _height(h)
	, _title(title)
	, _isInited(false)
	, _sceneManager(nullptr)
{
}

veVisualiser::~veVisualiser()
{
}
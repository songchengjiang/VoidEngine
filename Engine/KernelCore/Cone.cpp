#include "Cone.h"
#include "ConeRenderer.h"

veCone::veCone(veSceneManager *sm)
	: veRenderableObject(sm)
{
	_renderer = new veConeRenderer;
}

veCone::~veCone()
{

}


#include "Sphere.h"
#include "SphereRenderer.h"

veSphere::veSphere(veSceneManager *sm)
	: veRenderableObject(sm)
{
	_renderer = new veSphereRenderer;
}

veSphere::~veSphere()
{

}


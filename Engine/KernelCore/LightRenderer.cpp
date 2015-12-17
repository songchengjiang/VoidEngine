#include "LightRenderer.h"
#include "Node.h"
#include "Camera.h"

#define OCCLUSION_PASS 0
#define RENDERING_PASS 1

veDirectionalLightRenderer::veDirectionalLightRenderer()
{
}

veDirectionalLightRenderer::~veDirectionalLightRenderer()
{

}

void veDirectionalLightRenderer::immediatelyRender(veNode *node, vePass *pass, veCamera *camera) {
	updateBuffer();
	veRenderCommand rc;
	rc.mask = 0xffffffff;
	rc.worldMatrix = new veMat4Ptr(veMat4::IDENTITY);
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.renderer = this;
	rc.pass = pass;
	draw(rc);
}

vePointLightRenderer::vePointLightRenderer()
	: _lightVolumeScale(veMat4::IDENTITY)
{
}

vePointLightRenderer::~vePointLightRenderer()
{

}

void vePointLightRenderer::immediatelyRender(veNode *node, vePass *pass, veCamera *camera) {
	updateBuffer();
	veRenderCommand rc;
	rc.mask = 0xffffffff;
	rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix() * _lightVolumeScale);
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.renderer = this;
	rc.pass = pass;
	draw(rc);
}

veSpotLightRenderer::veSpotLightRenderer()
	: _lightVolumeScale(veMat4::IDENTITY)
{
}

veSpotLightRenderer::~veSpotLightRenderer()
{

}

void veSpotLightRenderer::immediatelyRender(veNode *node, vePass *pass, veCamera *camera) {
	updateBuffer();
	veRenderCommand rc;
	rc.mask = 0xffffffff;
	rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix() * _lightVolumeScale);
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.renderer = this;
	rc.pass = pass;
	draw(rc);
}
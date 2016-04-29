#include "LightRenderer.h"
#include "Node.h"
#include "Camera.h"

#define OCCLUSION_PASS 0
#define RENDERING_PASS 1

veScreenLightRenderer::veScreenLightRenderer()
{
}

veScreenLightRenderer::~veScreenLightRenderer()
{

}

void veScreenLightRenderer::render(veNode *node, vePass *pass, veCamera *camera) {
	updateBuffer();
	veRenderCommand rc;
	rc.mask = 0xffffffff;
	rc.worldMatrix = new veMat4Ptr(veMat4::IDENTITY);
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.renderer = this;
	rc.pass = pass;
	//draw(rc);
	camera->getRenderQueue()->pushCommand(0, veRenderQueue::RENDER_QUEUE_TRANSPARENT, rc);
}

veSphereLightRenderer::veSphereLightRenderer()
	: _lightVolumeScale(veMat4::IDENTITY)
{
}

veSphereLightRenderer::~veSphereLightRenderer()
{

}

void veSphereLightRenderer::render(veNode *node, vePass *pass, veCamera *camera) {
	updateBuffer();
	veRenderCommand rc;
	rc.mask = 0xffffffff;
	rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix() * _lightVolumeScale);
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.renderer = this;
	rc.pass = pass;
	//draw(rc);
	camera->getRenderQueue()->pushCommand(0, veRenderQueue::RENDER_QUEUE_TRANSPARENT, rc);
}

veConeLightRenderer::veConeLightRenderer()
	: _lightVolumeScale(veMat4::IDENTITY)
{
}

veConeLightRenderer::~veConeLightRenderer()
{

}

void veConeLightRenderer::render(veNode *node, vePass *pass, veCamera *camera) {
	updateBuffer();
	veRenderCommand rc;
	rc.mask = 0xffffffff;
	rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix() * _lightVolumeScale);
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.renderer = this;
	rc.pass = pass;
	//draw(rc);
	camera->getRenderQueue()->pushCommand(0, veRenderQueue::RENDER_QUEUE_TRANSPARENT, rc);
}
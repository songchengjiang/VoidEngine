#include "LightRenderer.h"
#include "Light.h"
#include "Camera.h"

#define OCCLUSION_PASS 0
#define RENDERING_PASS 1

veScreenLightRenderer::veScreenLightRenderer()
{
}

veScreenLightRenderer::~veScreenLightRenderer()
{

}

void veScreenLightRenderer::render(const veMat4 &transform, vePass *pass, veCamera *camera) {
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
{
}

veSphereLightRenderer::~veSphereLightRenderer()
{

}

void veSphereLightRenderer::render(const veMat4 &transform, vePass *pass, veCamera *camera) {
	updateBuffer();
	veRenderCommand rc;
	rc.mask = 0xffffffff;
	rc.worldMatrix = new veMat4Ptr(transform);
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.renderer = this;
	rc.pass = pass;
	//draw(rc);
	camera->getRenderQueue()->pushCommand(0, veRenderQueue::RENDER_QUEUE_TRANSPARENT, rc);
}

veConeLightRenderer::veConeLightRenderer()
{
}

veConeLightRenderer::~veConeLightRenderer()
{

}

void veConeLightRenderer::render(const veMat4 &transform, vePass *pass, veCamera *camera) {
	updateBuffer();
	veRenderCommand rc;
	rc.mask = 0xffffffff;
	rc.worldMatrix = new veMat4Ptr(transform);
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.renderer = this;
	rc.pass = pass;
	//draw(rc);
	camera->getRenderQueue()->pushCommand(0, veRenderQueue::RENDER_QUEUE_TRANSPARENT, rc);
}
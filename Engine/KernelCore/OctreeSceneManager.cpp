#include "OctreeSceneManager.h"
#include "Octree.h"
#include "OctreeNode.h"
#include "OctreeCamera.h"
#include "OctreeRenderQueue.h"
#include "RenderableObject.h"
#include "Ray.h"
#include "FileCore/File.h"
#include "Application.h"
#include "MaterialManager.h"

veOctreeSceneManager::veOctreeSceneManager()
	: _boundingBox(veVec3(-10000.0f), veVec3(10000.0f))
	, _octreeMaxDeep(8)
	, _octree(new veOctree)
	, _parallelUpdateOctantNum(0)
{
	init();
}

veOctreeSceneManager::veOctreeSceneManager(const veBoundingBox &bbox, unsigned int octreeDeep)
	: _boundingBox(bbox)
	, _octreeMaxDeep(octreeDeep)
	, _octree(new veOctree)
	, _parallelUpdateOctantNum(0)
{
	init();
}

veOctreeSceneManager::~veOctreeSceneManager()
{
}

void veOctreeSceneManager::init()
{
	_root = new veOctreeNode;
	_root->setSceneManager(this);
	_octree->originBoundingBox = _octree->boundingBox = _boundingBox;
}

veNode* veOctreeSceneManager::createNode(const std::string &name)
{
	auto node = new veOctreeNode;
	node->setName(name);
	node->setSceneManager(this);
	return node;
}

veCamera* veOctreeSceneManager::createCamera(const std::string &name, const veViewport &vp)
{
	auto camera = new veOctreeCamera(vp);
	camera->setName(name);
	camera->setSceneManager(this);
	_cameraList.push_back(camera);
	return camera;
}

void veOctreeSceneManager::requestRender(veNode *node)
{
	veOctreeNode *ocNode = static_cast<veOctreeNode *>(node);
	if (!ocNode) return;
	//{
	//	std::unique_lock<std::mutex> lock(_parallelUpdateOctantMutex);
	//	++_parallelUpdateOctantNum;
	//}
	//_threadPool.enqueue(nullptr, nullptr, [this, ocNode] {
		if (!ocNode->octant) {
			if (ocNode->getBoundingBox().isNull() || !ocNode->isIn(this->_octree->boundingBox)) {
				this->_octree->addNode(ocNode);
			}
			else {
				addOctreeNode(ocNode, this->_octree);
			}
		}
		else {
			if (!ocNode->isIn(ocNode->octant->boundingBox)) {
				ocNode->octant->removeNode(ocNode);

				if (!ocNode->isIn(this->_octree->boundingBox)) {
					this->_octree->addNode(ocNode);
				}
				else {
					addOctreeNode(ocNode, this->_octree);
				}
			}
		}
		//{
		//	std::unique_lock<std::mutex> lock(_parallelUpdateOctantMutex);
		//	--_parallelUpdateOctantNum;
		//	if (_parallelUpdateOctantNum == 0) {
		//		_parallelUpdateOctantCondition.notify_all();
		//	}
		//}
	//});
}

void veOctreeSceneManager::requestRayCast(veRay *ray)
{
	enqueueRequest([this, ray] {
		this->intersectByRay(_octree, ray);
		veSceneManager::requestRayCast(ray);
	});
}

bool veOctreeSceneManager::isNodeVisibleInScene(veNode *node)
{
	veOctreeNode *ocNode = static_cast<veOctreeNode *>(node);
	for (auto &cam : _cameraList) {
		if (cam->isInScene()) {
			veOctreeCamera *octreeCam = static_cast<veOctreeCamera *>(cam.get());
			if (octreeCam->isNodeVisibleInCamera(ocNode))
				return true;
		}
	}
	return false;
}

void veOctreeSceneManager::addOctreeNode(veOctreeNode *node, veOctree *octant, unsigned int depth)
{
	if (!octant) return;

	const veBoundingBox &bbox = node->getBoundingBox();
	if (depth < _octreeMaxDeep && octant->isTwiceSize(bbox)) {
		unsigned int x, y, z;
		octant->getChildIndex(bbox, x, y, z);
		if (!octant->getChild(x, y, z)) {
			auto octree = new veOctree;
			const veVec3& octantMin = octant->boundingBox.min();
			const veVec3& octantMax = octant->boundingBox.max();
			veVec3 &min = octree->originBoundingBox.min();
			veVec3 &max = octree->originBoundingBox.max();
			if (x == 0)
			{
				min.x() = octantMin.x();
				max.x() = (octantMin.x() + octantMax.x()) * 0.5f;
			}
			else
			{
				min.x() = (octantMin.x() + octantMax.x()) * 0.5f;
				max.x() = octantMax.x();
			}
			if (y == 0)
			{
				min.y() = octantMin.y();
				max.y() = (octantMin.y() + octantMax.y()) * 0.5f;
			}
			else
			{
				min.y() = (octantMin.y() + octantMax.y()) * 0.5f;
				max.y() = octantMax.y();
			}
			if (z == 0)
			{
				min.z() = octantMin.z();
				max.z() = (octantMin.z() + octantMax.z()) * 0.5f;
			}
			else
			{
				min.z() = (octantMin.z() + octantMax.z()) * 0.5f;
				max.z() = octantMax.z();
			}
			octree->boundingBox = octree->originBoundingBox;
			octant->setChild(x, y, z, octree);	
		}

		addOctreeNode(node, octant->getChild(x, y, z), ++depth);
	}
	else {
		octant->addNode(node);
	}
}

void veOctreeSceneManager::intersectByRay(veOctree *octant, veRay *ray)
{
	if (!octant) return;
	if (ray->isIntersectWith(octant->boundingBox)) {
		if (!octant->nodeList.empty()) {
			std::unique_lock<std::mutex> lock(octant->nodeListMutex);
			for (auto &iter : octant->nodeList) {
				if (iter->isVisible()) {
					if (ray->isIntersectWith(iter->getBoundingBox())) {
						for (unsigned int i = 0; i < iter->getRenderableObjectCount(); ++i) {
							if (iter->getRenderableObject(i)->isVisible())
								iter->getRenderableObject(i)->intersectWith(ray, iter);
						}
					}
				}
			}
		}

		for (unsigned int i = 0; i < 8; ++i) {
			if (octant->children[i]) {
				intersectByRay(octant->children[i], ray);
			}
		}
	}
}

void veOctreeSceneManager::update()
{
	_root->update(this, veMat4::IDENTITY);
	//std::unique_lock<std::mutex> lock(_parallelUpdateOctantMutex);
	//_parallelUpdateOctantCondition.wait(lock, [this] { return _parallelUpdateOctantNum == 0; });
	_octree->updateBoundingBox();
}

void veOctreeSceneManager::render()
{
	if (!veApplication::instance()->makeContextCurrent()) return;
	culling();

	if (!_lightList.empty()) {
		veRenderer::CURRENT_RENDER_STAGE = veRenderer::PRELIGHTING;
		for (auto &light : _lightList) {
			if (light->isShadowEnabled() && light->isVisible() && light->isInScene()) {
				if (light->getLightType() == veLight::POINT) {
					for (unsigned int i = 0; i < 6; ++i) {
						veOctreeCamera *cam = static_cast<veOctreeCamera *>(light->getShadowRenderingCamera(i));
						cam->renderingOctree();
						cam->render();
					}
				}
				else {
					veOctreeCamera *cam = static_cast<veOctreeCamera *>(light->getShadowRenderingCamera(0));
					cam->renderingOctree();
					cam->render();
				}
			}
		}
	}

	veRenderer::CURRENT_RENDER_STAGE = veRenderer::FRAMEBUFFER;
	for (auto &iter : _cameraList) {
		if (iter->isVisible() && iter->isInScene() && iter != _mainCamera) {
			if (iter->getFrameBufferObject()) {
				veOctreeCamera *rttCam = static_cast<veOctreeCamera *>(iter.get());
				//rttCam->walkingOctree(this->_octree);
				if (_skyBox.valid()) {
					_skyBox->render(rttCam);
				}
				rttCam->renderingOctree();
				rttCam->render();
			}
		}
	}

	veRenderer::CURRENT_RENDER_STAGE = veRenderer::RENDERING;
	if (_mainCamera.valid() && _mainCamera->isInScene() && _mainCamera->isVisible()) {
		veOctreeCamera *mainCam = static_cast<veOctreeCamera *>(_mainCamera.get());
		//mainCam->walkingOctree(this->_octree);
		if (_skyBox.valid()) {
			_skyBox->render(mainCam);
		}
		mainCam->renderingOctree();

		if (!_postProcesserList.empty()) {
			veRenderer::CURRENT_RENDER_STAGE = veRenderer::POSTPROCESS;
			if (!_postProcesserFBO.valid()) {
				_postProcesserFBO = veFrameBufferObjectManager::instance()->getOrCreateFrameBufferObject("postProcesserFBO");
			}
			mainCam->setFrameBufferObject(_postProcesserFBO.get());
			for (size_t i = 0; i < _postProcesserList.size(); ++i) {
				_postProcesserList[i]->process(_postProcesserFBO.get(), mainCam);
			}
			mainCam->setFrameBufferObject(nullptr);
		}
		mainCam->render();
	}
	veApplication::instance()->swapBuffers();
}

void veOctreeSceneManager::culling()
{
	for (auto &iter : _cameraList) {
		if (iter->isVisible() && iter->isInScene()) {
			veOctreeCamera *cam = static_cast<veOctreeCamera *>(iter.get());
			_threadPool.enqueue(nullptr, nullptr, [this, cam] {
				cam->walkingOctree(this->_octree);
			});
		}
	}

	if (!_lightList.empty()) {
		for (auto &light : _lightList) {
			if (light->isShadowEnabled() && light->isVisible() && light->isInScene()) {
				if (light->getLightType() == veLight::POINT) {
					for (unsigned int i = 0; i < 6; ++i) {
						veOctreeCamera *cam = static_cast<veOctreeCamera *>(light->getShadowRenderingCamera(i));
						_threadPool.enqueue(nullptr, nullptr, [this, cam] {
							cam->walkingOctree(this->_octree);
						});
					}
				}
				else {
					veOctreeCamera *cam = static_cast<veOctreeCamera *>(light->getShadowRenderingCamera(0));
					_threadPool.enqueue(nullptr, nullptr, [this, cam] {
						cam->walkingOctree(this->_octree);
					});
				}
			}
		}
	}
}
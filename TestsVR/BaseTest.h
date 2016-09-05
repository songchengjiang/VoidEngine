#ifndef _BASE_TEST_
#define _BASE_TEST_
#include "VoidEngine.h"
#include "VRCore/GearVRComponent.h"
#include "VRHelper.h"

#if defined(_MSC_VER)
#pragma warning( disable : 4996 )
#endif

#define EYE_NUM 2

#define MASTER_CAMERA_FLAG 1
#define SECOND_CAMERA_FLAG (1 << 1)

class LightUpdater : public veComponent
{
public:

	LightUpdater(veLight *light, float radius, float height)
		: _lastChangeColorTime(0.0)
        , _light(light){
		_angle = veMath::veRandomUnitization() * veMath::TWO_PI;
		_radius = radius;
		_height = height;
		_oriColor = _desColor = veVec3(veMath::veRandomUnitization(), veMath::veRandomUnitization(), veMath::veRandomUnitization());
	}
	virtual bool handle(veSceneManager *sm, veViewer *viewer, const veEvent &event) override{
		return false;
	}

	virtual void beforeUpdate(veSceneManager *sm) override{
		if (_attachedNodeList.empty()) return;

        updateColor(sm->getDeltaTime());
        updateMatrix(sm->getDeltaTime());
	}

private:

	void updateColor(double deltaTime) {
		veVec3 col = _oriColor * (1.0 - _lastChangeColorTime) + _desColor * _lastChangeColorTime;
		_light->setColor(col);

		if (1.0 < _lastChangeColorTime) {
			_oriColor = _desColor;
			_desColor = veVec3(veMath::veRandomUnitization(), veMath::veRandomUnitization(), veMath::veRandomUnitization());
			_lastChangeColorTime = 0.0f;
		}
		_lastChangeColorTime += deltaTime;
	}

	void updateMatrix(double deltaTime) {
		veReal x = _radius * veMath::veCos(_angle);
		veReal y = _radius * veMath::veSin(_angle);
		_light->getAttachedNodeList()[0]->setMatrix(veMat4::lookAt(veVec3(x, _height, y), veVec3::ZERO, veVec3::UNIT_Y));
		_angle += veMath::QUARTER_PI * deltaTime;
		_angle = fmod(_angle, veMath::TWO_PI);
	}

private:

	double _lastChangeColorTime;
	veVec3 _oriColor;
	veVec3 _desColor;
	veReal _angle;
	veReal _radius;
	veReal _height;
    veLight *_light;
};

class BaseTest
{
public:
	BaseTest(const veVRHelper &vrHelper) {
		veFile::instance()->addSearchPath("../resources/");
		_sceneManager = new veOctreeSceneManager(veBoundingBox(veVec3(-1000.0f), veVec3(1000.0f)), 8);
        _mainViewer = veApplication::instance()->getViewer(0);
        _mainViewer->setSceneManager(_sceneManager);
		int width = _mainViewer->width();
		int height = _mainViewer->height();

		auto gearVRComp = new veGearVRComponent(vrHelper.javaVM, vrHelper.activity);
		_mainViewer->addListener(gearVRComp);
		for (unsigned short eye = 0; eye < EYE_NUM; ++eye) {
			_eyesCamera[eye] = _sceneManager->createCamera(eye == 0? "LeftEyeCamera": "RightEyeCamera", {0, 0, width, height });
			_eyesCamera[eye]->setViewMatrixAslookAt(veVec3(0.0f, 0.0f, 10.0f), veVec3::ZERO, veVec3::UNIT_Y);
			_eyesCamera[eye]->setProjectionMatrixAsPerspective(90.0f, (float)width / (float)height, 1.0f, 1000.0f);
			_eyesCamera[eye]->setClearColor(veVec4(0.1f));
			_eyesCamera[eye]->addComponent(gearVRComp);
			_eyesCamera[eye]->setMask(eye == 0? MASTER_CAMERA_FLAG: SECOND_CAMERA_FLAG);
			_sceneManager->getRootNode()->addChild(_eyesCamera[eye]);
		}
		//_mainViewer->setCamera(_eyesCamera[0]);


		{
			veNode *node = _sceneManager->createNode("node");
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setPosition(veVec3::ZERO);
			transer->setScale(veVec3(0.01f));

			veTexture *texture = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, "textures/void.png", "void"));
			auto image = _sceneManager->createImage("image", texture);
			image->setType(veSurface::SURFACE);
			image->setAlphaThreshold(0.5f);
			node->addRenderableObject(image);

			_sceneManager->getRootNode()->addChild(node);
		}

//		std::string fontFile = "fonts/arial.ttf";
//		{
//			auto text = _sceneManager->createText("text0", new veFont(fontFile, 64), "Hello Void Engine");
//			text->setColor(veVec4(1.0f, 0.0f, 0.0f, 1.0f));
//			auto node = _sceneManager->createNode("node0");
//			node->addRenderableObject(text);
//			veTransformer *transer = new veTransformer;
//			node->addComponent(transer);
//			transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
//			_sceneManager->getRootNode()->addChild(node);
//		}


//		{
//			auto mats = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, "postprocessers/oldTV.vemtl", "oldTV-mats"));
//			auto postProcesser = _sceneManager->createPostProcesser("oldTV");
//			postProcesser->setMaterialArray(mats);
//			for (unsigned short eye = 0; eye < EYE_NUM; ++eye) {
//				_eyesCamera[eye]->addPostProcesser(postProcesser);
//			}
//		}

//		veAnimationContainer* animationContainer = static_cast<veAnimationContainer *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.veanim", "laoshu-anim"));
//		{
//			veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(_sceneManager, "models/laoshu_ani_v03.vem", "laoshu-0"));
//			//node->addComponent(new KeyboardInputer);
//			veTransformer *transer = new veTransformer;
//			entity->addComponent(transer);
//			//transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
//			transer->setScale(veVec3(0.3f));
//			transer->setPosition(veVec3::ZERO);
//			//transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_Y));
//			_sceneManager->getRootNode()->addChild(entity);
//
//			veAnimationPlayer* player = _sceneManager->createAnimationPlayer("player0", animationContainer);
//			player->start();
//			player->setLoopAnimation(true);
//			player->attachNode(entity);
//		}
	};
	~BaseTest() {
		VE_SAFE_DELETE(_sceneManager);
	};

	virtual void init() {
	}

protected:

	veSceneManager *_sceneManager;
	veCamera  *_eyesCamera[EYE_NUM];
    veViewer  *_mainViewer;
};

#endif
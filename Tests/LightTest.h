#ifndef _LIGHT_TEST_
#define _LIGHT_TEST_
#include "BaseTest.h"

class Updater : public veComponent 
{
public:

	Updater() 
		: _lastChangeColorTime(0.0){
		_angle = veMath::randomUnitization() * veMath::TWO_PI;
		_oriColor = _desColor = veVec3(veMath::randomUnitization(), veMath::randomUnitization(), veMath::randomUnitization());
	}
	virtual bool handle(veNode *node, veVisualiser *vs, const veEvent &event) {
		return false;
	}

	virtual void update(veNode *node, veVisualiser *vs) {
		auto light = static_cast<veLight *>(node);
		if (light) {
			auto param = light->getParameter("color");
			updateColor(param, vs->getDeltaTime());
			updateMatrix(light, vs->getDeltaTime());
		}
	}

private:

	void updateColor(veParameter *colorParam, double deltaTime) {
		veVec3 col = _oriColor * (1.0 - _lastChangeColorTime) + _desColor * _lastChangeColorTime;
		colorParam->set(col);

		if (1.0 < _lastChangeColorTime) {
			_oriColor = _desColor;
			_desColor = veVec3(veMath::randomUnitization(), veMath::randomUnitization(), veMath::randomUnitization());
			_lastChangeColorTime = 0.0f;
		}
		_lastChangeColorTime += deltaTime;
	}

	void updateMatrix(veLight *light, double deltaTime) {
		veReal radius = 5.0;
		veReal x = radius * veMath::cos(_angle);
		veReal y = radius * veMath::sin(_angle);
		veReal z = 5.0f;
		light->setMatrix(veMat4::lookAt(veVec3(x, y, z), veVec3::ZERO, veVec3::UNIT_Y));
		_angle += veMath::QUARTER_PI * deltaTime;
	}

private:

	double _lastChangeColorTime;
	veVec3 _oriColor;
	veVec3 _desColor;
	veReal _angle;
};

class LightTest : public BaseTest
{
public:
	LightTest() {
		veLightManager::instance()->loadLightTemplates("lights/point.velightemplates");

		veRenderableObject *obj = nullptr;
		veNode *root = new veNode;
		{
			veNode *node = static_cast<veNode *>(veFile::instance()->readFile("models/teapot.vem"));
			obj = node->getChild(0)->getChild(0)->getRenderableObject(0);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(2.0f));
			transer->setRotation(veQuat(veMath::HALF_PI, veVec3::UNIT_X));
			root->addChild(node);
		}

		{
			veNode *node = static_cast<veNode *>(veFile::instance()->readFile("models/plane.vem"));
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(20.0f));
			root->addChild(node);
		}

		{
			veNode *node = new veNode;
			node->addRenderableObject(obj);
			//node->addComponent(new KeyboardInputer);
			veTransformer *transer = new veTransformer;
			node->addComponent(transer);
			transer->setScale(veVec3(2.0f));
			transer->setRotation(veQuat(veMath::PI, veVec3::UNIT_X));
			root->addChild(node);
		}

		{
			veLight *directional0 = static_cast<veLight *>(veFile::instance()->readFile("lights/directional0.velight"));
			veTransformer *lightTranser = new veTransformer;
			directional0->addComponent(lightTranser);
			directional0->addComponent(new Updater);
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));
			root->addChild(directional0);
		}

		{
			veLight *point0 = static_cast<veLight *>(veFile::instance()->readFile("lights/point0.velight"));
			veTransformer *lightTranser = new veTransformer;
			point0->addComponent(lightTranser);
			point0->addComponent(new Updater);
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));
			root->addChild(point0);
		}

		{
			veLight *spot0 = static_cast<veLight *>(veFile::instance()->readFile("lights/spot0.velight"));
			veTransformer *lightTranser = new veTransformer;
			spot0->addComponent(lightTranser);
			spot0->addComponent(new Updater);
			lightTranser->setPosition(veVec3(0.0f, 0.0f, 5.0f));
			root->addChild(spot0);
		}

		_visualiser->setSceneNode(root);
	};
	~LightTest() {};

protected:


};

#endif
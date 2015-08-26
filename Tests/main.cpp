#include "VoidEngine.h"

class KeyboardInputer : public veComponent
{
public:

	KeyboardInputer(){
		//setEventFilter(veEvent::VE_KEYBOARD_EVENT);
	}
	~KeyboardInputer(){}

	virtual bool handle(veNode *node, veVisualiser *vs, const veEvent &event) override{
		static char outputString[512];
		sprintf(outputString, "Keyboard: (%c)", event.getKeySymbol());
		vePrinter::print(outputString);
		sprintf(outputString, "Mouse: (%f, %f)", event.getMouseX(), event.getMouseY());
		vePrinter::print(outputString);
		return true;
	}

private:

};

int main(int argc, char **argv)
{
	veNode *root = new veNode;
	veRenderableObject *obj = nullptr;
	{
		veNode *node = static_cast<veNode *>(veFile::instance()->readFile("models/plane.vem"));
		//node->addComponent(new KeyboardInputer);
		veTransformer *transer = new veTransformer;
		node->addComponent(transer);
		transer->setPosition(veVec3(0.0f, 0.0f, 0.0f));
		transer->setScale(veVec3(1.0f));
		transer->setRotation(veQuat(-veMath::HALF_PI, veVec3::UNIT_X));
		root->addChild(node);
		obj = node->getChild(0)->getChild(0)->getRenderableObject(0);
	}

	//{
	//	veNode *node = new veNode;
	//	veTransformer *transer = new veTransformer;
	//	node->addComponent(transer);
	//	transer->setPosition(veVec3(-1.0f, 0.0f, 0.0f));
	//	transer->setScale(veVec3(1.0f));
	//	node->addRenderableObject(obj);
	//	root->addChild(node);
	//}


	{
		auto visualiser = veDirector::DIRECTOR.createVisualiser(800, 600, "Game");
		visualiser->setSceneNode(root);
		visualiser->getCamera()->setViewMatrixAslookAt(veVec3(0.0f, 0.0f, 5.0f), veVec3::ZERO, veVec3::UNIT_Y);
	}

	//{
	//	auto visualiser = veDirector::instance()->createVisualiser(400, 300, "Game");
	//	visualiser->setSceneNode(node);
	//}

	return veDirector::DIRECTOR.run();
}

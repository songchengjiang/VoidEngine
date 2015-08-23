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
	veNode *node = static_cast<veNode *>(veFile::instance()->readFile("models/sphere.vem"));
	node->addComponent(new KeyboardInputer);
	veTransformer *transer = new veTransformer;
	node->addComponent(transer);
	transer->setPosition(veVec3(10.0f, 0.0f, 0.0f));
	transer->setScale(veVec3(0.5f));
	transer->setRotation(veQuat(veMath::QUARTER_PI, veVec3::UNIT_X));

	{
		auto visualiser = veDirector::DIRECTOR.createVisualiser(800, 600, "Game");
		visualiser->setSceneNode(node);
	}

	//{
	//	auto visualiser = veDirector::instance()->createVisualiser(400, 300, "Game");
	//	visualiser->setSceneNode(node);
	//}

	return veDirector::DIRECTOR.run();
}

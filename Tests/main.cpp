#include "VoidEngine.h"

class KeyboardInputer : public veComponent
{
public:

	KeyboardInputer(){
		//setEventFilter(veEvent::VE_KEYBOARD_EVENT);
	}
	~KeyboardInputer(){}

	virtual bool handle(double deltaTime, veNode *node, veVisualiser *vs, const veEvent &event) override{
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
	auto visualiser = veDirector::instance()->createVisualiser(800, 600, "Game");
	auto node = new veNode;

	node->addComponent(new KeyboardInputer);

	auto transer = new veTransformer;
	node->addComponent(transer);
	transer->setPosition(veVec3(10.0f, 0.0f, 0.0f));
	transer->setScale(veVec3(0.5f));
	transer->setRotation(veQuat(veMath::QUARTER_PI, veVec3::UNIT_X));

	auto meshRenderer = new veMeshRenderer;
	node->addComponent(meshRenderer);

	visualiser->setSceneNode(node);

	return veDirector::instance()->run();
}

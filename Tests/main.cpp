#include "VoidEngine.h"

class KeyboardInputer : public veInputer
{
public:

	KeyboardInputer(){
		//setFilter(veEvent::VE_WIN_EVENT);
	}
	~KeyboardInputer(){}

	virtual bool input(veNode *node, const veEvent &event, const veVisualiser *vs) override{
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
	node->addInputer(new KeyboardInputer);
	visualiser->setSceneNode(node);
	return veDirector::instance()->run();

}

#include "Node.h"
#include "Visualiser.h"

veNode::veNode()
{

}

veNode::~veNode()
{

}

int veNode::addChild(veNode *child)
{
	auto iter = std::find(_children.begin(), _children.end(), child);
	if (iter != _children.end()) return -1;
	_children.push_back(child);
	return _children.size() - 1;
}

bool veNode::removeChild(veNode *child)
{
	auto iter = std::find(_children.begin(), _children.end(), child);
	if (iter == _children.end()) return false;
	_children.erase(iter);
	return true;
}

veNode* veNode::removeChild(unsigned int cIndex)
{
	veAssert(cIndex < _children.size());
	veNode* child = _children[cIndex].get();
	_children.erase(_children.begin() + cIndex);
	return child;
}

veNode* veNode::getChild(unsigned int cIndex)
{
	veAssert(cIndex < _children.size());
	return _children[cIndex].get();
}

int veNode::addInputer(veInputer *inputer)
{
	auto iter = std::find(_inputerList.begin(), _inputerList.end(), inputer);
	if (iter != _inputerList.end()) return -1;
	_inputerList.push_back(inputer);
	return _inputerList.size() - 1;
}

bool veNode::removeInputer(veInputer *inputer)
{
	auto iter = std::find(_inputerList.begin(), _inputerList.end(), inputer);
	if (iter == _inputerList.end()) return false;
	_inputerList.erase(iter);
	return true;
}

veInputer* veNode::removeInputer(unsigned int inpIndex)
{
	veAssert(inpIndex < _inputerList.size());
	veInputer* inputer = _inputerList[inpIndex].get();
	_inputerList.erase(_inputerList.begin() + inpIndex);
	return inputer;
}

veInputer* veNode::getInputer(unsigned int inpIndex)
{
	veAssert(inpIndex < _inputerList.size());
	return _inputerList[inpIndex].get();
}

bool veNode::routeEvent(const veEvent &event, const veVisualiser *vs)
{
	for (auto &inputer : _inputerList) {
		if (event.getEventType() & inputer->getFilter()) {
			if (inputer->input(this, event, vs)) return true;
		}
	}

	if (!_children.empty()){
		for (auto &child : _children){
			if (child->routeEvent(event, vs)) return true;
		}
	}

	return false;
}

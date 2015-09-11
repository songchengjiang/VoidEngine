#include "Animation.h"
#include "Node.h"
#include "Visualiser.h"
#include "NodeVisitor.h"

class FindNode : public veNodeVisitor 
{
public:

	virtual void visit(veNode &node) {
		nodeList.push_back(&node);
	}

	std::vector<veNode *> nodeList;
};

veAnimKeyValues::veAnimKeyValues()
	: USE_VE_PTR_INIT
{

}

veAnimKeyValues::~veAnimKeyValues()
{

}

void veAnimKeyValues::setPosition(double keyTime, const veVec3 &pos)
{
	_positions[keyTime] = pos;
}

void veAnimKeyValues::setScale(double keyTime, const veVec3 &scl)
{
	_scales[keyTime] = scl;
}

void veAnimKeyValues::setRotation(double keyTime, const veQuat &rot)
{
	_rotations[keyTime] = rot;
}


bool veAnimKeyValues::evaluate(double keyTime, veVec3 &pos, veVec3 &scl, veQuat &rot)
{
	auto posIter = _positions.lower_bound(keyTime);
	auto rotIter = _rotations.lower_bound(keyTime);
	auto sclIter = _scales.lower_bound(keyTime);

	bool state = false;
	if (posIter != _positions.end()) {
		//auto prePosIter = posIter;
		//--prePosIter;
		//if (prePosIter != _positions.end()) {
		//	double t = (keyTime - prePosIter->first) / (posIter->first - prePosIter->first);
		//	pos = prePosIter->second * (1.0 - t) + posIter->second * t;
		//}
		pos = posIter->second;
		state = true;
	}

	if (rotIter != _rotations.end()) {
		//auto preRotIter = rotIter;
		//--preRotIter;
		//if (preRotIter != _rotations.end()) {
		//	double t = (keyTime - preRotIter->first) / (rotIter->first - preRotIter->first);
		//	rot = veQuat::slerp(t, preRotIter->second, rotIter->second);
		//}
		rot = rotIter->second;
		state = true;
	}

	if (sclIter != _scales.end()) {
		//auto preSclIter = sclIter;
		//--preSclIter;
		//if (preSclIter != _scales.end()) {
		//	double t = (keyTime - preSclIter->first) / (sclIter->first - preSclIter->first);
		//	scl = preSclIter->second * (1.0 - t) + sclIter->second * t;
		//}
		scl = sclIter->second;
		state = true;
	}

	return state;
}

veAnimation::veAnimation()
	: USE_VE_PTR_INIT
	, _duration(0.0)
	, _frameRate(0.0)
	, _needRefresh(true)
{

}

veAnimation::~veAnimation()
{

}

void veAnimation::update(veNode *node, double frame)
{
	if (_needRefresh) {
		FindNode fn;
		node->accept(fn);
		for (auto &iter : fn.nodeList) {
			auto valuse = getAnimKeyValuesByName(iter->getName());
			if (valuse) {
				_nodeAnims[iter] = valuse;
			}
		}
		_needRefresh = false;
	}

	if (!_nodeAnims.empty()) {
		for (auto &iter : _nodeAnims) {
			veVec3 pos;
			veVec3 scl(1.0f);
			veQuat rot;
			if (iter.second->evaluate(frame, pos, scl, rot)) {
				veMat4 nodeMat;
				nodeMat.makeTransform(pos, scl, rot);
				iter.first->setMatrix(nodeMat);
			}
		}
	}
}

void veAnimation::addAnimKeyValues(veAnimKeyValues *keyValues)
{
	_animsKeyValuse.push_back(keyValues);
	_needRefresh = true;
}

veAnimKeyValues* veAnimation::getAnimKeyValuesByName(const std::string &name)
{
	for (auto &iter : _animsKeyValuse) {
		if (iter->getName() == name)
			return iter.get();
	}
	return nullptr;
}

veAnimationContainer::veAnimationContainer()
	: _activeAnimationChannel(nullptr)
	, _smimulationFrame(0.0)
	, _startFrame(0)
	, _endFrame(-1)
	, _frameRate(0.0)
	, _needUpdate(false)
	, _isLoop(false)
{

}

veAnimationContainer::~veAnimationContainer()
{

}

void veAnimationContainer::update(veNode *node, veVisualiser *vs)
{
	if (_activeAnimationChannel && _needUpdate) {
		_activeAnimationChannel->update(node, _smimulationFrame);
		if (_smimulationFrame <= _endFrame)
			_smimulationFrame += vs->getDeltaTime() * _frameRate;
		else if (_isLoop){
			_smimulationFrame = _startFrame;
		}
	}
}

void veAnimationContainer::addAnimationChannel(veAnimation *anim)
{
	_animations.push_back(anim);
}

veAnimation* veAnimationContainer::getAnimationChannel(unsigned int idx)
{
	veAssert(idx < _animations.size());
	return _animations[idx].get();
}

void veAnimationContainer::setActiveAnimationChannel(unsigned int idx)
{
	veAssert(idx < _animations.size());
	_activeAnimationChannel = _animations[idx].get();
}

void veAnimationContainer::start(double sFrame, double eFrame)
{
	if (!_activeAnimationChannel && !_animations.empty()) {
		_activeAnimationChannel = _animations[0].get();
	}
	if (!_activeAnimationChannel) return;

	_startFrame = sFrame;
	_smimulationFrame = _startFrame;
	_endFrame = eFrame < 0.0 ? _activeAnimationChannel->getDuration() : eFrame;
	_frameRate = _frameRate <= 0.0 ? _activeAnimationChannel->getFrameRate() : _frameRate;
	_needUpdate = true;
}

void veAnimationContainer::pause()
{
	_needUpdate = false;
}

void veAnimationContainer::stop()
{
	_needUpdate = false;
	_smimulationFrame = 0.0;
}

#include "Animation.h"
#include "Node.h"
#include "Visualiser.h"
#include "NodeVisitor.h"
#include "SceneManager.h"
#include "Entity.h"
#include <set>

class FindMeshNodes 
{
public:

	void find(veMeshNode *node) {
		nodeList.push_back(node);
		for (size_t i = 0; i < node->getChildCount(); ++i) {
			find(node->getChild(i));
		}
	}

	std::vector<veMeshNode *> nodeList;
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
	else {
		if (!_positions.empty()) {
			pos = _positions.begin()->second;
			state = true;
		}
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
	else {
		if (!_rotations.empty()) {
			rot = _rotations.begin()->second;
			state = true;
		}
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
	else {
		if (!_scales.empty()) {
			scl = _scales.begin()->second;
			state = true;
		}
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

void veAnimation::update(veEntity *entity, double frame)
{
	if (_needRefresh) {
		FindMeshNodes finder;
		finder.find(entity->getRootMeshNode());
		for (auto &iter : finder.nodeList) {
			auto valuse = getAnimKeyValuesByName(iter->getName());
			if (valuse) {
				_boneAnims[iter] = valuse;
			}
		}
		_needRefresh = false;
	}

	if (!_boneAnims.empty()) {
		for (auto &iter : _boneAnims) {
			veVec3 pos;
			veVec3 scl(1.0);
			veQuat rot;
			if (iter.second->evaluate(frame, pos, scl, rot)) {
				veMat4 nodeMat;
				nodeMat.makeTransform(pos, scl, rot);
				iter.first->setMatrix(nodeMat);
			}
		}
		entity->dirtyBoundingBox();
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
	: USE_VE_PTR_INIT
	, _activeAnimationChannel(nullptr)
	, _smimulationFrame(0.0)
	, _startFrame(0)
	, _endFrame(-1)
	, _frameRate(0.0)
	, _needUpdate(false)
	, _requestNoUpdate(false)
	, _isLoop(false)
{

}

veAnimationContainer::~veAnimationContainer()
{

}

void veAnimationContainer::update(veNode *node, veEntity *entity, veSceneManager *sm)
{
	if (sm->isNodeVisibleInScene(node)) {
		if (_activeAnimationChannel && _needUpdate) {
			_activeAnimationChannel->update(entity, _smimulationFrame);
			if (_requestNoUpdate) _needUpdate = false;
		}
	}

	if (_smimulationFrame <= _endFrame)
		_smimulationFrame += sm->getDeltaTime() * _frameRate;
	else if (_isLoop) {
		_smimulationFrame = _startFrame;
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
	_requestNoUpdate = false;
}

void veAnimationContainer::pause()
{
	if (_needUpdate) {
		_startFrame = _smimulationFrame;
		_needUpdate = false;
		_requestNoUpdate = true;
	}
	else {
		_smimulationFrame = _startFrame;
		_needUpdate = true;
	}
}

void veAnimationContainer::stop()
{
	_requestNoUpdate = true;
	_smimulationFrame = 0.0;
}

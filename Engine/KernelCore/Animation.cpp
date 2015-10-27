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
{

}

veAnimation::~veAnimation()
{

}

void veAnimation::update(const BoneAnimations &bonesAnims, double simulationFrame)
{
	if (!bonesAnims.empty()) {
		for (auto &iter : bonesAnims) {
			veVec3 pos;
			veVec3 scl(1.0);
			veQuat rot;
			if (iter.second->evaluate(simulationFrame, pos, scl, rot)) {
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
{

}

veAnimationContainer::~veAnimationContainer()
{

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

double veAnimationContainer::getDuration() const
{
	double duration = 0.0;
	for (auto &anim : _animations) {
		duration = veMath::maximum(duration, anim->getDuration());
	}
	return duration;
}

double veAnimationContainer::getFrameRate() const
{
	double frameRate = 0.0;
	for (auto &anim : _animations) {
		frameRate = veMath::maximum(frameRate, anim->getFrameRate());
	}
	return frameRate;
}

veAnimationPlayer::veAnimationPlayer(veAnimationContainer *animationContainer)
	: USE_VE_PTR_INIT
	, _animationContainer(animationContainer)
	, _smimulationFrame(0.0)
	, _startFrame(0)
	, _pauseFrame(0)
	, _endFrame(-1)
	, _frameRate(0.0)
	, _needUpdate(false)
	, _requestNoUpdate(false)
	, _isLoop(false)
	, _entity(nullptr)
	, _manager(nullptr)
	, _activeAnimationChannel(nullptr)
{

}

veAnimationPlayer::~veAnimationPlayer()
{

}

void veAnimationPlayer::update(veSceneManager *sm)
{
	if (!_entity) return;
	if (_needUpdate) {
		if (!_activeAnimationChannel && _animationContainer->getAnimationChannelNum()) {
			_activeAnimationChannel = _animationContainer->getAnimationChannel(0);
		}
		for (auto &parentNode : _entity->getParents()) {
			if (sm->isNodeVisibleInScene(parentNode)) {
				updateAnimations();
				_entity->dirtyBoundingBox();
				if (_requestNoUpdate) _needUpdate = false;
				break;
			}
		}
	}

	if (_smimulationFrame <= _endFrame)
		_smimulationFrame += sm->getDeltaTime() * _frameRate;
	else if (_isLoop) {
		_smimulationFrame = _startFrame;
	}
}

void veAnimationPlayer::start(double sFrame /*= 0*/, double eFrame /*= -1*/)
{
	if (!_animationContainer.valid()) return;
	_startFrame = sFrame;
	_smimulationFrame = _startFrame;
	_endFrame = eFrame < 0.0 ? _animationContainer->getDuration() : eFrame;
	_frameRate = _frameRate <= 0.0 ? _animationContainer->getFrameRate() : _frameRate;
	_needUpdate = true;
	_requestNoUpdate = false;
}

void veAnimationPlayer::pause()
{
	if (_needUpdate) {
		_pauseFrame = _smimulationFrame;
		_needUpdate = false;
		_requestNoUpdate = true;
	}
	else {
		_smimulationFrame = _pauseFrame;
		_needUpdate = true;
		_requestNoUpdate = false;
	}
}

void veAnimationPlayer::stop()
{
	_requestNoUpdate = true;
	_smimulationFrame = 0.0;
}

void veAnimationPlayer::attachEntity(veEntity *entity)
{
	_entity = entity;
	if (_entity && _animationContainer.valid()) {
		_animationMap.clear();
		FindMeshNodes finder;
		finder.find(entity->getRootMeshNode());
		for (size_t i = 0; i < _animationContainer->getAnimationChannelNum(); ++i) {
			veAnimation *animation = _animationContainer->getAnimationChannel(i);
			for (auto &iter : finder.nodeList) {
				auto valuse = animation->getAnimKeyValuesByName(iter->getName());
				if (valuse) {
					_animationMap[animation].push_back(std::make_pair(iter, valuse));
				}
			}
		}
	}
}

void veAnimationPlayer::updateAnimations()
{
	auto iter = _animationMap.find(_activeAnimationChannel);
	if (iter != _animationMap.end()) {
		_activeAnimationChannel->update(iter->second, _smimulationFrame);
	}
}
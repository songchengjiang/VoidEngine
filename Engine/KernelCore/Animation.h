#ifndef _VE_ANIMATION_
#define _VE_ANIMATION_
#include "Prerequisites.h"
#include "VE_Ptr.h"
#include "Component.h"
#include "BaseCore/Array.h"
#include "BaseCore/Vector3.h"
#include "BaseCore/Quaternion.h"
#include <map>

class veEntity;
class veMeshNode;
class veAnimKeyValues;
class veAnimation;
class veAnimationManager;
typedef std::pair<veNode*, veAnimKeyValues*> BoneAnimationKeyValues;
typedef std::vector<BoneAnimationKeyValues> BoneAnimations;
typedef std::map< veAnimation*, BoneAnimations> AnimationMap;

class VE_EXPORT veAnimKeyValues
{
public:
	veAnimKeyValues();
	~veAnimKeyValues();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	void setPosition(double keyTime, const veVec3 &pos);
	void setScale(double keyTime, const veVec3 &scl);
	void setRotation(double keyTime, const veQuat &rot);

	bool evaluate(double keyTime, veVec3 &pos, veVec3 &scl, veQuat &rot);

private:

	std::map<double, veVec3> _positions;
	std::map<double, veVec3> _scales;
	std::map<double, veQuat> _rotations;
};

class VE_EXPORT veAnimation
{
	friend class veAnimationManager;
public:
	~veAnimation();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	void addAnimKeyValues(veAnimKeyValues *keyValues);
	void setDuration(double duration) { _duration = duration; };
	double getDuration() const { return _duration; }
	void setFrameRate(double fRate) { _frameRate = fRate; }
	double getFrameRate() const { return _frameRate; }

	void update(const BoneAnimations &bonesAnims, double simulationFrame);

	veAnimKeyValues* getAnimKeyValuesByName(const std::string &name);

private:

	veAnimation();

private:

	std::vector< VE_Ptr<veAnimKeyValues> > _animsKeyValuse;
	double _duration;
	double _frameRate;
};

class VE_EXPORT veAnimationContainer
{
	friend class veAnimationManager;
public:
	~veAnimationContainer();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	void addAnimationChannel(veAnimation *anim);
	veAnimation* getAnimationChannel(size_t idx);
	size_t getAnimationChannelNum() const { return _animations.size(); }

	double getDuration() const;
	double getFrameRate() const;

private:

	veAnimationContainer();

private:

	std::vector<VE_Ptr<veAnimation>> _animations;
};

class VE_EXPORT veAnimationPlayer
{
	friend class veAnimationManager;
public:
	~veAnimationPlayer();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	void update(veSceneManager *sm);

	void start(double sFrame = 0, double eFrame = -1);
	void pause();
	void stop();
	void setFrameRate(double fRate) { _frameRate = fRate; }
	double getFrameRate() const { return _frameRate; }
	void setLoopAnimation(bool isLoop) { _isLoop = isLoop; }
	bool isLoopAnimation() { return _isLoop; }

	void attachNode(veNode *node);

	veAnimationContainer* getAnimationContainer() { return _animationContainer.get(); }
	void setActiveAnimationChannel(veAnimation *animation) { _activeAnimationChannel = animation; }

private:

	veAnimationPlayer(veAnimationContainer *animationContainer);
	void updateAnimations();

private:

	VE_Ptr<veAnimationContainer> _animationContainer;
	AnimationMap _animationMap;
	veNode * _node;
	veAnimation* _activeAnimationChannel;
	double _smimulationFrame;
	double _startFrame;
	double _pauseFrame;
	double _endFrame;
	double _frameRate;
	bool   _needUpdate;
	bool   _requestNoUpdate;
	bool   _isLoop;

	veAnimationManager *_manager;
};

#endif
#ifndef _VE_ANIMATION_
#define _VE_ANIMATION_
#include "Prerequisites.h"
#include "VE_Ptr.h"
#include "Component.h"
#include "BaseCore/Array.h"
#include "BaseCore/Vector3.h"
#include "BaseCore/Quaternion.h"
#include <map>

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
public:
	veAnimation();
	~veAnimation();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	void addAnimKeyValues(veAnimKeyValues *keyValues);
	void setDuration(double duration) { _duration = duration; };
	double getDuration() const { return _duration; }
	void setFrameRate(double fRate) { _frameRate = fRate; }
	double getFrameRate() const { return _frameRate; }

	void update(veNode *node, double frame);

private:

	veAnimKeyValues* getAnimKeyValuesByName(const std::string &name);

private:

	std::vector< VE_Ptr<veAnimKeyValues> > _animsKeyValuse;
	std::map< veNode*, veAnimKeyValues* > _nodeAnims;
	double _duration;
	double _frameRate;
	bool _needRefresh;
};

class VE_EXPORT veAnimationContainer : public veComponent
{
public:
	veAnimationContainer();
	~veAnimationContainer();

	virtual void update(veNode *node, veSceneManager *sm) override;

	void addAnimationChannel(veAnimation *anim);
	veAnimation* getAnimationChannel(unsigned int idx);
	size_t getAnimationChannelNum() const { return _animations.size(); }

	void setActiveAnimationChannel(unsigned int idx);

	void start(double sFrame = 0, double eFrame = -1);
	void pause();
	void stop();
	void setFrameRate(double fRate) { _frameRate = fRate; }
	double getFrameRate() const { return _frameRate; }
	void setLoopAnimation(bool isLoop) { _isLoop = isLoop; }
	bool isLoopAnimation() { return _isLoop; }

private:

	std::vector<VE_Ptr<veAnimation>> _animations;
	veAnimation* _activeAnimationChannel;
	double _smimulationFrame;
	double _startFrame;
	double _endFrame;
	double _frameRate;
	bool   _needUpdate;
	bool   _requestNoUpdate;
	bool   _isLoop;
};

#endif
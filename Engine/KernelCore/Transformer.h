#ifndef _VE_TRANSFORMER_
#define _VE_TRANSFORMER_
#include "Component.h"
#include "BaseCore/Vector3.h"
#include "BaseCore/Quaternion.h"

class VE_EXPORT veTransformer : public veComponent
{
public:
	veTransformer();
	~veTransformer();

	virtual void update(double deltaTime, veNode *node, veVisualiser *vs) override;

	void setPosition(const veVec3 &pos);
	const veVec3& getPosition() const;
	void setRotation(const veQuat &rot);
	const veQuat& getRotation() const;
	void setScale(const veVec3 &scl);
	const veVec3& getScale() const;

	void translate(const veVec3 &pos);
	void rotate(const veQuat &rot);
	void scale(const veVec3 &scl);

private:

	veVec3 _position;
	veQuat _rotation;
	veVec3 _scale;
	bool _neeUpdate;
};

#endif

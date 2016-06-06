#ifndef _VE_COMPONENT_
#define _VE_COMPONENT_
#include "Prerequisites.h"
#include "Event.h"
#include "VE_Ptr.h"

class veNode;
class veSceneManager;
class VE_EXPORT veComponent
{
public:
	veComponent();
	virtual ~veComponent();

	USE_VE_PTR;

	virtual bool handle(veSceneManager *sm, const veEvent &event) { return false; }
    virtual void beforeUpdate(veSceneManager *sm) {}
	virtual void afterUpdate(veSceneManager *sm) {}
    virtual void beforeRender(veSceneManager *sm) {}
    virtual void afterRender(veSceneManager *sm) {}
	virtual void onAttachToNode(veNode *node);
	virtual void onDetachToNode(veNode *node);

	void setEventFilter(const veEvent::EventType filter) { _filter = filter; };
	veEvent::EventType getEventFilter() const { return _filter; };
    
    void setEnable(bool isEnable) { _isEnabled = isEnable; }
    bool getEnabled() { return _isEnabled; }


protected:

	veEvent::EventType _filter;
    bool _isEnabled;
	std::vector<veNode *> _attachedNodeList;
};

typedef std::vector< VE_Ptr<veComponent> > veComponentList;

#endif

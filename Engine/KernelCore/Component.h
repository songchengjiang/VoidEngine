#ifndef _VE_COMPONENT_
#define _VE_COMPONENT_
#include "Prerequisites.h"
#include "Event.h"
#include "VE_Ptr.h"

class veNode;
class veCamera;
class veSceneManager;
class veViewer;
class VE_EXPORT veComponent
{
public:
	veComponent();
	virtual ~veComponent();

	USE_VE_PTR;
    USE_NAME_PROPERTY;

    virtual void created() {}
    virtual void destroyed() {}
	virtual bool handle(veSceneManager *sm, veViewer *viewer, const veEvent &event) { return false; }
    virtual void update(veSceneManager *sm) {}
    virtual void start(veSceneManager *sm) {}
    virtual void beforeRender(veSceneManager *sm, veViewer *viewer) {}
    virtual void afterRender(veSceneManager *sm, veViewer *viewer) {}
	virtual bool onAttachToNode(veNode *node);
	virtual bool onDetachToNode(veNode *node);
    veNode* getAttachedNode() const { return _attachedNode; }

	void setEventFilter(const veEvent::EventType filter) { _filter = filter; };
	veEvent::EventType getEventFilter() const { return _filter; };
    
    void setEnable(bool isEnable) { _isEnabled = isEnable; }
    bool isEnabled() { return _isEnabled; }
    void setUpdateOrder(int order) { _updateOrder = order; }
    int getUpdateOrder() const { return _updateOrder; }


protected:

	veEvent::EventType _filter;
    bool               _isEnabled;
	veNode *           _attachedNode;
    int                _updateOrder;
};

typedef std::vector< VE_Ptr<veComponent> > veComponentList;

#endif

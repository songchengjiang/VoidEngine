#ifndef _VE_EVENT_DISPATCHER_
#define _VE_EVENT_DISPATCHER_
#include "Prerequisites.h"
#include "Event.h"

class veViewer;
class VE_EXPORT veEventDispatcher
{
public:
    typedef std::unordered_map<veViewer *, Events> EventList;
    veEventDispatcher();
    ~veEventDispatcher();
    
    void addEvent(veViewer *viewer, const veEvent &event);
    EventList& getEventList() { return _eventList; }
    const EventList& getEventList() const { return _eventList; }
    void clearEventList() { _eventList.clear(); }
    
    
private:
    
    EventList       _eventList;
};

#endif

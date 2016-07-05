#include "EventDispatcher.h"
#include "SceneManager.h"

veEventDispatcher::veEventDispatcher()
{

}

veEventDispatcher::~veEventDispatcher()
{

}

void veEventDispatcher::addEvent(veViewer *viewer, const veEvent &event)
{
    _eventList[viewer].push_back(event);
}
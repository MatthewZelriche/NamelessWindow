#include "NamelessWindow/Events/Event.hpp"

using namespace NLSWIN;

EventType KeyEvent::type = typeid(KeyEvent);
EventType WindowCloseEvent::type = typeid(WindowCloseEvent);
EventType WindowFocusedEvent::type = typeid(WindowFocusedEvent);
#include "NamelessWindow/Event.hpp"

using namespace NLSWIN;

std::type_index KeyEvent::type           = typeid(KeyEvent);
std::type_index WindowCloseEvent::type   = typeid(WindowCloseEvent);
std::type_index WindowFocusedEvent::type = typeid(WindowFocusedEvent);
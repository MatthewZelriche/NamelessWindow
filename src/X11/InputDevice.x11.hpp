#pragma once

#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include "NamelessWindow/NLSAPI.h"

struct NLSWIN_API_PRIVATE XI2EventMask {
   xcb_input_event_mask_t head;
   xcb_input_xi_event_mask_t mask;
};

namespace NLSWIN {}
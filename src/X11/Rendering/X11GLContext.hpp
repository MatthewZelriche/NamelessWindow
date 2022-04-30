/*!
 * @file
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 *
 * @addtogroup X11 Linux X11 API
 * @brief Platform-specific X11 implementation of the API
 */
#pragma once
#include <GL/glx.h>
#include <xcb/xcb.h>

#include <vector>

#include "../X11Window.hpp"
#include "NamelessWindow/Rendering/GLContext.hpp"

namespace NLSWIN {

/*! @ingroup X11 */
class NLSWIN_API_PRIVATE X11GLContext : public GLContext {
   public:
   void MakeContextCurrent() override;
   void SwapContextBuffers() override;

   X11GLContext(std::weak_ptr<const X11Window> window);

   private:
   std::weak_ptr<const X11Window> m_xcbWindow;
   xcb_window_t m_xcbWindowID {0};
   GLXContext m_context {nullptr};
   GLXWindow m_glxWindow {0};
   GLXFBConfig m_chosenConfig {nullptr};
};

}  // namespace NLSWIN
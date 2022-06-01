/*!
 * @file
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 *
 * @addtogroup Common Public API
 * @brief Documentation for public API that clients directly interact with.
 */
#pragma once
#include <memory>

#include "../NLSAPI.hpp"
#include "../Window.hpp"

namespace NLSWIN {

/*!
 * @brief Represents an OpenGL Context
 * @ingroup Common
 * @headerfile "Rendering/GLContext.hpp"
 *
 * An OpenGL Context is associated with a particular instance of a window. This window is the window that this
 * context will draw to. The Context must always have a valid Window, else undefined behavior occurs.
 * @see Window
 * @see GLConfiguration
 */
class NLSWIN_API_PUBLIC GLContext {
   public:
   /*!
    * @brief Construct a new OpenGL Context.
    * @param window The window that this context will be drawn to.
    * @throws PlatformInitializationException
    * @throws RenderContextInitFailureException
    * @return A unique pointer to the newly constructed Window. Caller owns this resource and is expected to
    * manage its lifetime.
    * @warning It is the caller's responsibility to ensure the lifetime of this object never exceeds the
    * lifetime of the associated Window object.
    * @see EventDispatcher
    */
   static std::unique_ptr<GLContext> Create(const std::shared_ptr<const Window> window);

   /**
    * @brief Set this context to be the currently active context.
    * @throws InvalidRenderContextStateException if this method is called after its associated window has been
    * destroyed.
    */
   virtual void MakeContextCurrent() = 0;
   /**
    * @brief Swap the buffers for this context.
    * @throws InvalidRenderContextStateException if this method is called after its associated window has been
    * destroyed.
    */
   virtual void SwapContextBuffers() = 0;

   virtual ~GLContext() = default;
};

}  // namespace NLSWIN
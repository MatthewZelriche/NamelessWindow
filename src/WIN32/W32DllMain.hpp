#pragma once
/*!
 * @file
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 *
 * @addtogroup WIN32 Windows API
 * @brief Platform-specific Windows implementation of the API
 */
#include <windows.h>

namespace NLSWIN {

/*! @brief The module handle for the NLSWIN DLL.
 * @ingroup WIN32
 */
// extern HINSTANCE dllInstance;

HINSTANCE GetDLLInstanceHandle();

}  // namespace NLSWIN
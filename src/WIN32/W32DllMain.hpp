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
extern HINSTANCE dllInstance;
/*! @brief Used only to get the module handle of NLSWIN's DLL.
 * @ingroup WIN32
 */
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);

}  // namespace NLSWIN
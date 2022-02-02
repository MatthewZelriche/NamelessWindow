#include "W32DllMain.hpp"

namespace NLSWIN {
HINSTANCE dllInstance;
}

BOOL WINAPI NLSWIN::DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
   dllInstance = hinstDLL;
}
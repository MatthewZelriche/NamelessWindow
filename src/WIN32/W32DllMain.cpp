#include "W32DllMain.hpp"

namespace NLSWIN {

static HINSTANCE dllInstance;

extern "C" BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
   dllInstance = hinstDLL;

   return true;
}

HINSTANCE GetDLLInstanceHandle() {
   return dllInstance;
}
}  // namespace NLSWIN
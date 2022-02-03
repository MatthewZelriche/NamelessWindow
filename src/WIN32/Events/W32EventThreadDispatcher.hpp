/*!
 * @file
 * @date 2021-2022
 * @copyright Modified form of dtc, originally authored by Casey Muratori. Full license follows:
 * MIT License
 *
 * Copyright (c) 2021 Casey Muratori
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * @sa https://github.com/cmuratori/dtc
 *
 * @addtogroup WIN32 Windows API
 * @brief Platform-specific Windows implementation of the API
 */
#pragma once

#include <windows.h>

#include "NamelessWindow/NLSAPI.hpp"

namespace NLSWIN {

struct Win32CreationProps {
   DWORD dwExStyle;
   LPCSTR lpClassName;
   LPCSTR lpWindowName;
   DWORD dwStyle;
   int X;
   int Y;
   int nWidth;
   int nHeight;
   HWND hWndParent;
   HMENU hMenu;
   HINSTANCE hInstance;
   LPVOID lpParam;
};

struct WParamWithWindowHandle {
   WPARAM wParam {0};
   HWND sourceWindow {nullptr};
};

const unsigned int CREATE_NLSWIN_WINDOW = WM_USER + 0x1000;
const unsigned int DESTROY_NLSWIN_WINDOW = WM_USER + 0x1001;

class NLSWIN_API_PRIVATE W32EventThreadDispatcher {
   public:
   static void Initialize();
   static inline HWND GetDispatcherHandle() { return m_dispatcherHandle; }

   static DWORD WINAPI EventThreadMain(LPVOID Param);
   static LRESULT CALLBACK WindowBuilder(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam);
   static LRESULT CALLBACK DispatchProc(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam);

   private:
   static HWND m_dispatcherHandle;
   static DWORD m_mainThreadID;
   static DWORD m_eventThreadID;
};
}  // namespace NLSWIN

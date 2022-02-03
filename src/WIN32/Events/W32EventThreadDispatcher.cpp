/* Modified form of dtc, originally authored by Casey Muratori. Full license follows:
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
 */
#include "W32EventThreadDispatcher.hpp"

#include "../W32DllMain.hpp"

using namespace NLSWIN;

HWND W32EventThreadDispatcher::m_dispatcherHandle {nullptr};
DWORD W32EventThreadDispatcher::m_mainThreadID {0};
DWORD W32EventThreadDispatcher::m_eventThreadID {0};

void W32EventThreadDispatcher::Initialize() {
   // Construct a fake window just for the handle so we can create a thread with the win32 api.
   WNDCLASS windowClass = {0};
   windowClass.hInstance = GetDLLInstanceHandle();
   windowClass.lpfnWndProc = DefWindowProcA;
   windowClass.lpszClassName = "Dummy";
   RegisterClassA(&windowClass);
   HWND dummyHandle = CreateWindowExA(0, windowClass.lpszClassName, "Dummy", 0, 0, 0, 0, 0, nullptr, nullptr,
                                      GetDLLInstanceHandle(), nullptr);

   m_mainThreadID = GetCurrentThreadId();

   // Create the thread that will exist only to handle our event loop.
   CreateThread(0, 0, &EventThreadMain, dummyHandle, 0, &m_eventThreadID);

   // Don't need the window any longer.
   DestroyWindow(dummyHandle);

   // Block until we are sure we've constructed the input handler window.
   while (!m_dispatcherHandle) {}
   int a = 0;
}

DWORD WINAPI W32EventThreadDispatcher::EventThreadMain(LPVOID Param) {
   // Create our fake "Dispatcher" window
   WNDCLASS windowClass = {0};
   windowClass.lpfnWndProc = &WindowBuilder;
   windowClass.hInstance = GetDLLInstanceHandle();
   windowClass.lpszClassName = "Dispatcher";
   RegisterClassA(&windowClass);
   m_dispatcherHandle = CreateWindowExA(0, windowClass.lpszClassName, "Dispatcher", 0, 0, 0, 0, 0, nullptr,
                                        nullptr, GetDLLInstanceHandle(), nullptr);

   while (true) {
      MSG Message;
      GetMessageA(&Message, 0, 0, 0);
      TranslateMessage(&Message);
      // TODO: Decide what queued messages to send.
      if (false) {
         PostThreadMessageA(m_mainThreadID, Message.message, Message.wParam, Message.lParam);
      } else {
         DispatchMessageA(&Message);
      }
   }
}

LRESULT CALLBACK W32EventThreadDispatcher::DispatchProc(HWND Window, UINT Message, WPARAM WParam,
                                                        LPARAM LParam) {
   LRESULT Result = 0;
   // Don't have to worry about a delete because this var is static.
   static WParamWithWindowHandle *wParamWithH = new WParamWithWindowHandle;
   wParamWithH->wParam = WParam;
   wParamWithH->sourceWindow = Window;
   // TODO: What non-queued messages to send?
   switch (Message) {
      case WM_CLOSE: {
         PostThreadMessageA(m_mainThreadID, Message, (WPARAM)wParamWithH, LParam);
         return 0;
      }
      default: {
         Result = DefWindowProcA(Window, Message, WParam, LParam);
      } break;
   }

   return Result;
}

LRESULT CALLBACK W32EventThreadDispatcher::WindowBuilder(HWND Window, UINT Message, WPARAM WParam,
                                                         LPARAM LParam) {
   LRESULT Result = 0;

   switch (Message) {
      case CREATE_NLSWIN_WINDOW: {
         auto windowProps = reinterpret_cast<Win32CreationProps *>(WParam);
         Result = (LRESULT)CreateWindowExA(
            windowProps->dwExStyle, windowProps->lpClassName, windowProps->lpWindowName, windowProps->dwStyle,
            windowProps->X, windowProps->Y, windowProps->nWidth, windowProps->nHeight,
            windowProps->hWndParent, windowProps->hMenu, windowProps->hInstance, windowProps->lpParam);
         break;
      }
      case DESTROY_NLSWIN_WINDOW: {
         DestroyWindow((HWND)WParam);
         break;
      }
      default: {
         Result = DefWindowProcA(Window, Message, WParam, LParam);
         break;
      }
   }
   return Result;
}
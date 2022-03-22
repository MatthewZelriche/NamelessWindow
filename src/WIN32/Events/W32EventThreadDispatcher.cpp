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
#include "../W32Util.hpp"

using namespace NLSWIN;

HWND W32EventThreadDispatcher::m_dispatcherHandle {nullptr};
DWORD W32EventThreadDispatcher::m_mainThreadID {0};
DWORD W32EventThreadDispatcher::m_eventThreadID {0};

void W32EventThreadDispatcher::Initialize() {
   // TODO: Is this how we should handle high DPI?
   SetProcessDPIAware();
   // Construct a fake window just for the handle so we can create a thread with the win32 api.
   WNDCLASSW windowClass = {0};
   windowClass.hInstance = GetDLLInstanceHandle();
   windowClass.lpfnWndProc = DefWindowProcA;
   windowClass.lpszClassName = L"Dummy";
   RegisterClassW(&windowClass);
   HWND dummyHandle = CreateWindowExW(0, windowClass.lpszClassName, L"Dummy", 0, 0, 0, 0, 0, nullptr, nullptr,
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
   WNDCLASSW windowClass = {0};
   windowClass.lpfnWndProc = &WindowBuilder;
   windowClass.hInstance = GetDLLInstanceHandle();
   windowClass.lpszClassName = L"Dispatcher";
   RegisterClassW(&windowClass);

   m_dispatcherHandle = CreateWindowExW(0, windowClass.lpszClassName, L"Dispatcher", 0, 0, 0, 0, 0, nullptr,
                                        nullptr, GetDLLInstanceHandle(), nullptr);

   while (true) {
      MSG Message;
      GetMessageW(&Message, 0, 0, 0);
      TranslateMessage(&Message);
      // TODO: Decide what queued messages to send.
      if (false) {
         PostThreadMessageW(m_mainThreadID, Message.message, Message.wParam, Message.lParam);
      } else {
         DispatchMessageW(&Message);
      }
   }
}

LRESULT CALLBACK W32EventThreadDispatcher::DispatchProc(HWND Window, UINT Message, WPARAM WParam,
                                                        LPARAM LParam) {
   LRESULT Result = 0;
   // TODO: What non-queued messages to send?
   switch (Message) {
      case WM_CLOSE: {
         PostThreadEvent(Window, Message, WParam, LParam);
         return 0;
      }
      case WM_SIZE: {
         PostThreadEvent(Window, Message, WParam, LParam);
         break;
      }
      case WM_INPUT: {
         PostThreadEvent(Window, Message, WParam, LParam);
         return 0;
      }
      case WM_SETFOCUS: {
         PostThreadEvent(Window, Message, (WPARAM)GetFocus(), LParam);
         return 0;
      }
      case WM_MOVE: {
         PostThreadEvent(Window, Message, WParam, LParam);
         return 0;
      }
      default: {
         Result = DefWindowProcW(Window, Message, WParam, LParam);
      } break;
   }

   return Result;
}

void W32EventThreadDispatcher::PostThreadEvent(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam) {
   WParamWithWindowHandle *wParamWithH = new WParamWithWindowHandle;
   wParamWithH->wParam = WParam;
   wParamWithH->sourceWindow = Window;
   PostThreadMessageW(m_mainThreadID, Message, (WPARAM)wParamWithH, LParam);
}

LRESULT CALLBACK W32EventThreadDispatcher::WindowBuilder(HWND Window, UINT Message, WPARAM WParam,
                                                         LPARAM LParam) {
   LRESULT Result = 0;

   switch (Message) {
      case CREATE_NLSWIN_WINDOW: {
         auto windowProps = reinterpret_cast<Win32CreationProps *>(WParam);
         Result = (LRESULT)CreateWindowExW(
            windowProps->dwExStyle, windowProps->className.c_str(), windowProps->windowName.c_str(),
            windowProps->dwStyle, windowProps->X, windowProps->Y, windowProps->nWidth, windowProps->nHeight,
            windowProps->hWndParent, windowProps->hMenu, windowProps->hInstance, windowProps->lpParam);
         break;
      }
      case DESTROY_NLSWIN_WINDOW: {
         DestroyWindow((HWND)WParam);
         break;
      }
      default: {
         Result = DefWindowProcW(Window, Message, WParam, LParam);
         break;
      }
   }
   return Result;
}
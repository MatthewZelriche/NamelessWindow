if (${NLSWIN_X11})
   set(NLSWIN_SOURCE_FILES "X11/X11EventListener.cpp"
                           "X11/X11EventBus.cpp"
                           "X11/XConnection.cpp"
                           "X11/X11Window.cpp"
                           "X11/X11RawInputDevice.cpp"
                           "X11/X11InputDevice.cpp"
                           "X11/X11Keyboard.cpp"
                           "X11/X11GenericMouse.cpp"
                           "X11/X11RawMouse.cpp"
                           "X11/X11Cursor.cpp"
                           "X11/X11Util.cpp"
                           "X11/Rendering/X11GLContext.cpp")
elseif(${NLSWIN_WAYLAND})

elseif(${NLSWIN_WIN32})
   set(NLSWIN_SOURCE_FILES "WIN32/W32DllMain.cpp"
                           "WIN32/W32Window.cpp"
                           "WIN32/Events/W32EventListener.cpp"
                           "WIN32/Events/W32EventBus.cpp"
                           "WIN32/Events/W32EventThreadDispatcher.cpp")
else()
   message(FATAL_ERROR "Unrecognized build target!")
endif()

add_library(NamelessWindow SHARED ${NLSWIN_SOURCE_FILES})
target_compile_definitions(NamelessWindow PRIVATE BUILDING_NLSWIN_LIB)
target_include_directories(NamelessWindow PRIVATE "${PROJECT_SOURCE_DIR}/include" ${NLSWIN_PLATFORMSPECIFIC_INCLUDES} ${NLSWIN_THIRDPARTY_INCLUDES})
target_link_libraries(NamelessWindow ${NLSWIN_LIBRARIES_TO_LINK})
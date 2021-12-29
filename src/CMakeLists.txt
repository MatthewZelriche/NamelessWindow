if (${NLSWIN_X11})
   set(NLSWIN_SOURCE_FILES "X11/Window.x11.cpp"
                           "X11/EventListener.x11.cpp"
                           "X11/EventQueue.x11.cpp"
                           "X11/XConnection.cpp"
                           "X11/InputDevice.x11.cpp"
                           "X11/Keyboard.x11.cpp"
                           "X11/InputMapper.x11.cpp"
                           "X11/PointerDevice.x11.cpp"
                           "X11/MasterPointer.x11.cpp"
                           "X11/RawPointer.x11.cpp")
elseif(${NLSWIN_WAYLAND})

elseif(${NLSWIN_WIN32})

else()
   message(FATAL_ERROR "Unrecognized build target!")
endif()

add_library(NamelessWindow SHARED "Common/Event.cpp" ${NLSWIN_SOURCE_FILES})
target_include_directories(NamelessWindow PRIVATE "${PROJECT_SOURCE_DIR}/include" ${NLSWIN_PLATFORMSPECIFIC_INCLUDES} ${NLSWIN_THIRDPARTY_INCLUDES})
target_compile_definitions(NamelessWindow PRIVATE BUILDING_NLSWIN_LIB)
target_link_libraries(NamelessWindow ${NLSWIN_LIBRARIES_TO_LINK})
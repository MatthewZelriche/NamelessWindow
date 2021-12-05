if (${NLSWIN_X11})
   set(NLSWIN_SOURCE_FILES "Window.x11.cpp"
                           "EventListener.x11.cpp"
                           "XConnection.cpp"
elseif(${NLSWIN_WAYLAND})

elseif(${NLSWIN_WIN32})

else()
   message(FATAL_ERROR "Unrecognized build target!")
endif()

add_library(NamelessWindow SHARED "Common/Event.cpp" ${NLSWIN_SOURCE_FILES})
target_include_directories(NamelessWindow PRIVATE "${PROJECT_SOURCE_DIR}/include" ${NLSWIN_THIRDPARTY_INCLUDES})
target_compile_definitions(NamelessWindow PRIVATE BUILDING_NLSWIN_LIB)
target_link_libraries(NamelessWindow ${X11_xcb_LIB} xcb-randr xcb-xinput)
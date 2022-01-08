if (${NLSWIN_X11})
   set(NLSWIN_SOURCE_FILES "X11/X11EventListener.cpp"
                           "X11/X11EventBus.cpp"
                           "X11/XConnection.cpp"
                           "X11/X11Window.cpp")
elseif(${NLSWIN_WAYLAND})

elseif(${NLSWIN_WIN32})

else()
   message(FATAL_ERROR "Unrecognized build target!")
endif()

add_library(NamelessWindow SHARED ${NLSWIN_SOURCE_FILES})
target_include_directories(NamelessWindow PRIVATE "${PROJECT_SOURCE_DIR}/include" ${NLSWIN_PLATFORMSPECIFIC_INCLUDES} ${NLSWIN_THIRDPARTY_INCLUDES})
target_compile_definitions(NamelessWindow PRIVATE BUILDING_NLSWIN_LIB)
target_link_libraries(NamelessWindow ${NLSWIN_LIBRARIES_TO_LINK})
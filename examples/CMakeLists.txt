add_executable(Example_HelloWindow "HelloWorld.cpp")
target_include_directories(Example_HelloWindow PRIVATE "${PROJECT_SOURCE_DIR}/include/" ${NLSWIN_THIRDPARTY_INCLUDES})
target_link_libraries(Example_HelloWindow NamelessWindow)


add_executable(TestBed "TestBed.cpp")
target_include_directories(TestBed PRIVATE "${PROJECT_SOURCE_DIR}/include/" ${NLSWIN_THIRDPARTY_INCLUDES})
target_link_libraries(TestBed NamelessWindow)

add_executable(Imgui "Imgui.cpp"
					 "${PROJECT_SOURCE_DIR}/vendor/imgui/imgui.cpp"
					 "${PROJECT_SOURCE_DIR}/vendor/imgui/imgui_demo.cpp"
					 "${PROJECT_SOURCE_DIR}/vendor/imgui/imgui_draw.cpp"
					 "${PROJECT_SOURCE_DIR}/vendor/imgui/imgui_tables.cpp"
					 "${PROJECT_SOURCE_DIR}/vendor/imgui/imgui_widgets.cpp"
					 "../Extra/imgui_impl_nlswin.cpp"
					 "${PROJECT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_opengl3.cpp")
target_include_directories(Imgui PUBLIC "${PROJECT_SOURCE_DIR}/Vendor/imgui" "${PROJECT_SOURCE_DIR}/Vendor/glad")
target_include_directories(Imgui PUBLIC "${PROJECT_SOURCE_DIR}/include/" ${NLSWIN_THIRDPARTY_INCLUDES})
target_link_libraries(Imgui NamelessWindow)
add_executable(Example_HelloWindow "HelloWorld.cpp")
target_include_directories(Example_HelloWindow PRIVATE "${PROJECT_SOURCE_DIR}/include/" ${NLSWIN_THIRDPARTY_INCLUDES})
target_link_libraries(Example_HelloWindow NamelessWindow)


add_executable(TestBed "TestBed.cpp")
target_include_directories(TestBed PRIVATE "${PROJECT_SOURCE_DIR}/include/" ${NLSWIN_THIRDPARTY_INCLUDES})
target_link_libraries(TestBed NamelessWindow)
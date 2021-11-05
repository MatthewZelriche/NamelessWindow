add_executable(Example_HelloWindow "main.cpp")
target_include_directories(Example_HelloWindow PRIVATE "${PROJECT_SOURCE_DIR}/include/")
target_link_libraries(Example_HelloWindow NamelessWindow)
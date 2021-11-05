add_library(NamelessWindow SHARED "Test.cpp")
target_include_directories(NamelessWindow PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_compile_definitions(NamelessWindow PRIVATE BUILDING_NLSWIN_LIB)
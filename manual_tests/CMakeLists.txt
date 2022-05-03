add_executable(Test_WindowCreation "WindowCreation.cpp")
target_include_directories(Test_WindowCreation PRIVATE "${PROJECT_SOURCE_DIR}/include/" ${NLSWIN_THIRDPARTY_INCLUDES})
target_link_libraries(Test_WindowCreation NamelessWindow)

add_executable(Test_WindowEvents "WindowEvents.cpp")
target_include_directories(Test_WindowEvents PRIVATE "${PROJECT_SOURCE_DIR}/include/" ${NLSWIN_THIRDPARTY_INCLUDES})
target_link_libraries(Test_WindowEvents NamelessWindow)

add_executable(Test_MasterKeyboard "MasterKeyboard.cpp")
target_include_directories(Test_MasterKeyboard PRIVATE "${PROJECT_SOURCE_DIR}/include/" ${NLSWIN_THIRDPARTY_INCLUDES})
target_link_libraries(Test_MasterKeyboard NamelessWindow)

add_executable(Test_MultipleKeyboards "MultipleKeyboards.cpp")
target_include_directories(Test_MultipleKeyboards PRIVATE "${PROJECT_SOURCE_DIR}/include/" ${NLSWIN_THIRDPARTY_INCLUDES})
target_link_libraries(Test_MultipleKeyboards NamelessWindow)
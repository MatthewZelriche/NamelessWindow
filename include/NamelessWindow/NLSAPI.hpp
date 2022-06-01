/*!
 * @file Exceptions.hpp
 * @author MZelriche
 * @date 2021-2022
 * @copyright MIT License
 */
#pragma once

#ifdef _WIN32
#   ifdef BUILDING_NLSWIN_LIB
#      define NLSWIN_API_PUBLIC __declspec(dllexport)
#   else
#      define NLSWIN_API_PUBLIC __declspec(dllimport)
#   endif
#   define NLSWIN_API_PRIVATE
#elif __gnu_linux__
#   define NLSWIN_API_PUBLIC  __attribute__((visibility("default")))
#   define NLSWIN_API_PRIVATE __attribute__((visibility("hidden")))
#endif
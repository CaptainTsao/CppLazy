cmake_minimum_required(VERSION 3.19)
project(CppLazy LANGUAGES CXX C)

set(CppLazy_warning_guard "SYSTEM")
if (CppLazy_INCLUDE_WITHOUT_SYSTEM)
    set(CppLazy_warning_guard "")
endif ()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# ---- Import {fmt} ----
option(CppLazy_USE_STANDALONE "Standalone library without {fmt}" YES)
option(CppLazy_USE_INSTALLED_FMT "Import {fmt} using find_package" NO)

if (CppLazy_USE_STANDALONE)
    message(STATUS "Using cpp-lazy standalone version")
    add_compile_definitions(LZ_STANDALONE)
else()
    message(STATUS "Using cpp-lazy with {fmt} library")
    if (CppLazy_USE_INSTALLED_FMT)
        find_package(fmt 8 REQUIRED CONFIG)
    else()
        set(FMT_INSTALL YES CACHE INTERNAL "" FORCE)
        set(FMT_OS YES CACHE INTERNAL "")
        include(FetchContent)
        FetchContent_Declare(fmt
                GIT_REPOSITORY https://github.com/fmtlib/fmt.git
                GIT_TAG 9e8b86fd2d9806672cc73133d21780dd182bfd24
                UPDATE_DISCONNECTED YES)
        FetchContent_MakeAvailable(fmt)
    endif()
endif()

# ---- Declare library ----
add_library(CppLazy INTERFACE)
add_library(CppLazy::CppLazy ALIAS CppLazy)

target_link_libraries(CppLazy INTERFACE fmt::fmt)

target_compile_features(CppLazy INTERFACE cxx_std_11)

target_include_directories(CppLazy
#        ${CppLazy_warning_guard}
        INTERFACE
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>")
#include_directories("${PROJECT_SOURCE_DIR}/include/Lazy/Detail")
add_library(CppLzay main.cpp
        include/Lazy/detail/FunctionContainer.hpp
        include/Lazy/detail/LazyTools.hpp
        include/Lazy/detail/MapIterator.hpp
        include/Lazy/detail/FlattenIterator.hpp include/Lazy/detail/FilterIterator.hpp include/Lazy/detail/BasicIteratorView.hpp include/Lazy/detail/SplitIterator.hpp include/Lazy/detail/UniqueIterator.hpp include/Lazy/Map.hpp include/Lazy/Flatten.hpp)
#set(CMAKE_CXX_STANDARD 20)

#add_executable(CppLazy main.cpp include/Lazy/detail/LazyTools.hpp)

# ---- Install ----
#include(CMakePackageConfigHelpers)
#include(GNUInstallDirs)
#
#set(cpp-lazy_directory "cpp-lazy-${PROJECT_VERSION}")
#set(cpp-lazy_include_directory "${CMAKE_INSTALL_INCLUDEDIR}/${cpp-lazy_directory}")
#
#install(TARGETS CppLazy
#        EXPORT CppLazyTargets
#        INCLUDES
#        DESTINATION "${CppLazy_include_directory}")
#
#write_basic_package_version_file(
#        CppLazyConfigVersion.cmake
#        VERSION ${PROJECT_VERSION}
#        COMPATIBILITY SameMajorVersion
#        ARCH_INDEPENDENT)
#
#set(CppLazy_install_cmakedir "${CMAKE_INSTALL_LIBDIR}/cmake/${CppLazy_directory}")
#
#install(EXPORT CppLazyTargets
#        NAMESPACE CppLazy::
#        DESTINATION "${CppLazy_install_cmakedir}")
#
#install(FILES
#        "${PROJECT_SOURCE_DIR}/cmake/CppLazyConfig.cmake"
#        "${PROJECT_BINARY_DIR}/CppLazyConfigVersion.cmake"
#        DESTINATION "${CppLazy_install_cmakedir}")
#
#install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
#        DESTINATION "${CppLazy_include_directory}")
#
#set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.md")
#
#include(CPack)
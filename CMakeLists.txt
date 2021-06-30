cmake_minimum_required(VERSION 3.19)
project(CppLazy
        VERSION 1.0.0
        DESCRIPTION
        "A fast c++11 header only library for lazy evaluation and function tools"
        LANGUAGES CXX)

set(CppLazy_warning_guard "SYSTEM")
if (CppLazy_INCLUDE_WITHOUT_SYSTEM)
    set(CppLazy_warning_guard "")
endif ()


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
        ${CppLazy_warning_guard}
        INTERFACE
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>")

set(CMAKE_CXX_STANDARD 11)

#add_executable(CppLazy main.cpp include/Lazy/detail/LazyTools.hpp)
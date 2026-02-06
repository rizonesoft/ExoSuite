# ==============================================================================
# ExoSuite - Modern Windows Control Panel
# ==============================================================================
# Tech Stack: C++23, WinUI3, C++/WinRT (No .NET Framework Required)
# Target: Windows 10/11 64-bit only
# Build System: CMake 3.28+ with Visual Studio 2026 (v144)
# ==============================================================================

cmake_minimum_required(VERSION 3.28)

# ==============================================================================
# Project Definition
# ==============================================================================
project(ExoSuite
    VERSION 0.1.0
    DESCRIPTION "Modern Windows Control Panel with extensible architecture"
    HOMEPAGE_URL "https://rizonesoft.com"
    LANGUAGES CXX
)

# ==============================================================================
# Build Configuration
# ==============================================================================

# Enforce 64-bit builds only
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "ExoSuite requires 64-bit compilation. 32-bit is not supported.")
endif()

# C++23 Standard (required)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Visual Studio 2026 specific settings
if(MSVC)
    # Verify we're using VS2026 (v144) or compatible
    if(MSVC_VERSION LESS 1950)
        message(WARNING "ExoSuite is optimized for Visual Studio 2026 (v144). You are using an older version.")
    endif()
    
    # Compiler flags for VS2026
    add_compile_options(
        /W4                     # Warning level 4
        /WX-                    # Warnings not as errors (for development)
        /permissive-            # Strict C++ conformance
        /Zc:__cplusplus         # Correct __cplusplus macro value
        /Zc:preprocessor        # Use conforming preprocessor
        /utf-8                  # UTF-8 source and execution character set
        /EHsc                   # Standard C++ exception handling
        /MP                     # Multi-processor compilation
    )
    
    # Release-specific optimizations
    add_compile_options("$<$<CONFIG:Release>:/O2>")     # Maximum optimization
    add_compile_options("$<$<CONFIG:Release>:/GL>")     # Whole program optimization
    add_compile_options("$<$<CONFIG:Release>:/Gy>")     # Function-level linking
    
    # Debug-specific options
    add_compile_options("$<$<CONFIG:Debug>:/Od>")       # No optimization
    add_compile_options("$<$<CONFIG:Debug>:/Zi>")       # Debug information
    add_compile_options("$<$<CONFIG:Debug>:/RTC1>")     # Runtime checks
    
    # Linker flags
    add_link_options("$<$<CONFIG:Release>:/LTCG>")      # Link-time code generation
    add_link_options("$<$<CONFIG:Release>:/OPT:REF>")   # Remove unreferenced code
    add_link_options("$<$<CONFIG:Release>:/OPT:ICF>")   # Identical COMDAT folding
endif()

# ==============================================================================
# Windows SDK and Platform Configuration
# ==============================================================================

# Require Windows 10 SDK
set(CMAKE_SYSTEM_VERSION "10.0" CACHE STRING "Windows SDK version")

# Target Windows 10 20H1 (build 19041) as minimum
add_compile_definitions(
    WINVER=0x0A00
    _WIN32_WINNT=0x0A00
    NTDDI_VERSION=0x0A000007
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    UNICODE
    _UNICODE
)

# ==============================================================================
# Output Directories
# ==============================================================================

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Extensions output to extensions/ folder
set(EXOSUITE_EXTENSIONS_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/extensions)

# ==============================================================================
# C++/WinRT Configuration (for WinUI3)
# ==============================================================================

# TODO: Configure C++/WinRT and Windows App SDK when implementing UI
# find_package(Microsoft.Windows.CppWinRT CONFIG REQUIRED)
# find_package(Microsoft.WindowsAppSDK CONFIG REQUIRED)

# ==============================================================================
# Source Files
# ==============================================================================

# Main application source directory
set(EXOSUITE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Placeholder for main executable (uncomment when source files exist)
# add_executable(ExoSuite WIN32
#     ${EXOSUITE_SRC_DIR}/main.cpp
# )

# ==============================================================================
# Installation Configuration
# ==============================================================================

# Install rules (TODO: Configure when ready for distribution)
# install(TARGETS ExoSuite DESTINATION bin)
# install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/extensions DESTINATION bin)

# ==============================================================================
# Summary
# ==============================================================================

message(STATUS "")
message(STATUS "==============================================================================")
message(STATUS "ExoSuite Configuration Summary")
message(STATUS "==============================================================================")
message(STATUS "  Version:        ${PROJECT_VERSION}")
message(STATUS "  C++ Standard:   C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Generator:      ${CMAKE_GENERATOR}")
message(STATUS "  Build Type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler:       ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Platform:       ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}")
message(STATUS "  Architecture:   ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "==============================================================================")
message(STATUS "")

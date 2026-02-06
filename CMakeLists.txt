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

# Enforce Windows 10+ (build system check)
if(WIN32)
    cmake_host_system_information(RESULT WIN_VERSION QUERY WINDOWS_REGISTRY
        "HKLM/SOFTWARE/Microsoft/Windows NT/CurrentVersion"
        VALUE "CurrentBuildNumber")
    if(WIN_VERSION AND WIN_VERSION LESS 10240)
        message(FATAL_ERROR "ExoSuite requires Windows 10 or later. Detected build: ${WIN_VERSION}")
    endif()
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

# Extensions output to system/ folder in app root
set(EXOSUITE_SYSTEM_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/system)

# ==============================================================================
# Extension Helper Function
# ==============================================================================

# Helper function to create extension DLL projects that output to system/ folder
function(add_exosuite_extension TARGET_NAME)
    # Create shared library (DLL)
    add_library(${TARGET_NAME} SHARED ${ARGN})
    
    # Output to system/ folder
    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${EXOSUITE_SYSTEM_OUTPUT_DIRECTORY}
        LIBRARY_OUTPUT_DIRECTORY ${EXOSUITE_SYSTEM_OUTPUT_DIRECTORY}
        ARCHIVE_OUTPUT_DIRECTORY ${EXOSUITE_SYSTEM_OUTPUT_DIRECTORY}
        # Per-config output directories
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${EXOSUITE_SYSTEM_OUTPUT_DIRECTORY}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${EXOSUITE_SYSTEM_OUTPUT_DIRECTORY}
        LIBRARY_OUTPUT_DIRECTORY_DEBUG ${EXOSUITE_SYSTEM_OUTPUT_DIRECTORY}
        LIBRARY_OUTPUT_DIRECTORY_RELEASE ${EXOSUITE_SYSTEM_OUTPUT_DIRECTORY}
    )
    
    # Static runtime for consistency with main executable
    if(MSVC)
        set_property(TARGET ${TARGET_NAME} PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
    
    # Common include directories
    target_include_directories(${TARGET_NAME} PRIVATE ${EXOSUITE_SRC_DIR})
    
    # Common Windows libraries
    target_link_libraries(${TARGET_NAME} PRIVATE
        user32
        gdi32
        shell32
        ole32
        oleaut32
        advapi32
    )
endfunction()

# ==============================================================================
# C++/WinRT and Windows App SDK Configuration (WinUI3)
# ==============================================================================

# NuGet package versions
set(CPPWINRT_VERSION "2.0.240405.15" CACHE STRING "C++/WinRT version")
set(WINDOWSAPPSDK_VERSION "1.6.241114003" CACHE STRING "Windows App SDK version")

# NuGet packages directory
set(NUGET_PACKAGES_DIR "${CMAKE_BINARY_DIR}/packages")

# Option to enable WinUI3 (requires NuGet packages to be restored)
option(EXOSUITE_ENABLE_WINUI3 "Enable WinUI3 UI framework" OFF)

if(EXOSUITE_ENABLE_WINUI3)
    # C++/WinRT package paths
    set(CPPWINRT_ROOT "${NUGET_PACKAGES_DIR}/Microsoft.Windows.CppWinRT.${CPPWINRT_VERSION}")
    set(WINDOWSAPPSDK_ROOT "${NUGET_PACKAGES_DIR}/Microsoft.WindowsAppSDK.${WINDOWSAPPSDK_VERSION}")
    
    # Verify packages exist
    if(NOT EXISTS "${CPPWINRT_ROOT}")
        message(WARNING "C++/WinRT package not found. Run 'nuget restore' or disable EXOSUITE_ENABLE_WINUI3")
    endif()
    
    if(NOT EXISTS "${WINDOWSAPPSDK_ROOT}")
        message(WARNING "Windows App SDK package not found. Run 'nuget restore' or disable EXOSUITE_ENABLE_WINUI3")
    endif()
    
    # C++/WinRT compiler settings
    add_compile_definitions(
        WINRT_LEAN_AND_MEAN
        _SILENCE_CLANG_COROUTINE_MESSAGE
    )
    
    # Include paths for Windows App SDK
    # These will be populated when packages are restored
    set(WINDOWSAPPSDK_INCLUDE_DIRS
        "${WINDOWSAPPSDK_ROOT}/include"
    )
    
    set(WINDOWSAPPSDK_LIB_DIRS
        "${WINDOWSAPPSDK_ROOT}/lib/win10-x64"
    )
    
    message(STATUS "WinUI3 support: ENABLED")
    message(STATUS "  C++/WinRT version: ${CPPWINRT_VERSION}")
    message(STATUS "  Windows App SDK version: ${WINDOWSAPPSDK_VERSION}")
else()
    message(STATUS "WinUI3 support: DISABLED (enable with -DEXOSUITE_ENABLE_WINUI3=ON)")
endif()

# ==============================================================================
# Helper function for WinUI3 executables
# ==============================================================================

function(add_winui3_executable TARGET_NAME)
    if(NOT EXOSUITE_ENABLE_WINUI3)
        message(FATAL_ERROR "WinUI3 is not enabled. Set EXOSUITE_ENABLE_WINUI3=ON")
    endif()
    
    # Create Windows GUI executable
    add_executable(${TARGET_NAME} WIN32 ${ARGN})
    
    # Add Windows App SDK include directories
    target_include_directories(${TARGET_NAME} PRIVATE ${WINDOWSAPPSDK_INCLUDE_DIRS})
    
    # Link Windows App SDK libraries
    target_link_directories(${TARGET_NAME} PRIVATE ${WINDOWSAPPSDK_LIB_DIRS})
    
    # Common Windows libraries
    target_link_libraries(${TARGET_NAME} PRIVATE
        WindowsApp
        user32
        gdi32
        shell32
        ole32
        oleaut32
        advapi32
    )
    
    # Set Windows App SDK manifest
    set_target_properties(${TARGET_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
        VS_WINDOWS_TARGET_PLATFORM_MIN_VERSION "10.0.17763.0"
    )
endfunction()

# ==============================================================================
# Source Files
# ==============================================================================

# Main application source directory
set(EXOSUITE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Collect source files
set(EXOSUITE_SOURCES
    ${EXOSUITE_SRC_DIR}/main.cpp
    ${EXOSUITE_SRC_DIR}/Core/Platform.cpp
)

set(EXOSUITE_HEADERS
    ${EXOSUITE_SRC_DIR}/Core/Platform.h
)

# ==============================================================================
# Main Executable
# ==============================================================================

add_executable(ExoSuite WIN32
    ${EXOSUITE_SOURCES}
    ${EXOSUITE_HEADERS}
)

# Include directories
target_include_directories(ExoSuite PRIVATE ${EXOSUITE_SRC_DIR})

# Link Windows libraries
target_link_libraries(ExoSuite PRIVATE
    user32
    gdi32
    shell32
    ole32
    oleaut32
    advapi32
    ntdll
)

# Single executable configuration - static runtime for standalone deployment
if(MSVC)
    # Use static runtime library for Release (no VCRUNTIME dependency)
    set_property(TARGET ExoSuite PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Set executable properties
set_target_properties(ExoSuite PROPERTIES
    OUTPUT_NAME "ExoSuite"
    WIN32_EXECUTABLE TRUE
    VS_WINDOWS_TARGET_PLATFORM_MIN_VERSION "10.0.17763.0"
)

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

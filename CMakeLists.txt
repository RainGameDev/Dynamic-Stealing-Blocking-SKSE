cmake_minimum_required(VERSION 3.21)

# ---- Project ----------------------------------------------------------------

project(
    MySKSEPlugin
    VERSION 0.0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Dependencies -----------------------------------------------------------

# CommonLibSSE-NG is pulled in by vcpkg (see vcpkg.json / vcpkg-configuration.json).
find_package(CommonLibSSE CONFIG REQUIRED)

# ---- Plugin target ----------------------------------------------------------

# Gather every source under src/.
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

# add_commonlibsse_plugin generates the SKSE plugin metadata (SKSEPlugin_Version,
# the version resource, etc.) from the arguments below and produces a .dll.
add_commonlibsse_plugin(
    ${PROJECT_NAME}
    SOURCES ${SOURCES}
    AUTHOR "Your Name"
    # Supports Skyrim SE (1.5.x), AE (1.6.x) and VR from a single binary.
    USE_ADDRESS_LIBRARY
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_precompile_headers(${PROJECT_NAME} PRIVATE src/PCH.h)

# ---- MSVC options -----------------------------------------------------------

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /utf-8
        /Zc:preprocessor
        /W4
    )
    # /permissive- is intentionally omitted: CommonLibSSE-NG's headers rely on
    # MSVC's permissive template parsing, and under clang-cl (Linux cross build)
    # /permissive- disables -fms-compatibility, breaking those headers.
endif()

# ---- Optional: auto-deploy into a mod folder --------------------------------
# Set MYSKSE_OUTPUT_DIR (e.g. your MO2 mods folder) to copy the built DLL there.

if(DEFINED MYSKSE_OUTPUT_DIR AND NOT MYSKSE_OUTPUT_DIR STREQUAL "")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${MYSKSE_OUTPUT_DIR}/SKSE/Plugins"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:${PROJECT_NAME}>"
            "${MYSKSE_OUTPUT_DIR}/SKSE/Plugins/"
        COMMENT "Deploying ${PROJECT_NAME}.dll to ${MYSKSE_OUTPUT_DIR}"
    )
endif()

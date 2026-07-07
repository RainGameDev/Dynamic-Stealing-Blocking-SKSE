cmake_minimum_required(VERSION 3.21)

# ---- Project ----------------------------------------------------------------

project(
    DynamicStealingBlockingSKSE
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

# ---- Package the mod (DLL + translation files) ------------------------------
# Translation files are plain UTF-16 text loaded by the game at runtime; they are
# not compiled, just bundled next to the DLL in the mod's folder layout:
#
#     <mod>/SKSE/Plugins/<Plugin>.dll
#     <mod>/Interface/Translations/<Plugin>_<language>.txt
#
# After every build, assemble that layout under build/mod/ so it is ready to zip
# and upload as-is.

set(DSB_STAGE_DIR "${CMAKE_BINARY_DIR}/mod")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${DSB_STAGE_DIR}/SKSE/Plugins"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:${PROJECT_NAME}>" "${DSB_STAGE_DIR}/SKSE/Plugins/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Interface" "${DSB_STAGE_DIR}/Interface"
    COMMENT "Staging mod (DLL + translations) in ${DSB_STAGE_DIR}"
)

# Optional: also copy the finished layout straight into a mod-manager folder.
# Configure with -DDSB_OUTPUT_DIR="/path/to/MO2/mods/DynamicStealingBlockingSKSE".
if(DEFINED DSB_OUTPUT_DIR AND NOT DSB_OUTPUT_DIR STREQUAL "")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${DSB_OUTPUT_DIR}/SKSE/Plugins"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:${PROJECT_NAME}>" "${DSB_OUTPUT_DIR}/SKSE/Plugins/"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Interface" "${DSB_OUTPUT_DIR}/Interface"
        COMMENT "Deploying mod to ${DSB_OUTPUT_DIR}"
    )
endif()

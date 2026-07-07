# Cross-compile toolchain: build Windows x64 (MSVC ABI) binaries on Linux using
# clang-cl + lld-link against an xwin-provided Windows SDK / MSVC CRT.
#
# Requires XWIN_DIR to point at an `xwin splat` output directory containing
# crt/ and sdk/ subfolders.

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

if(NOT DEFINED XWIN_DIR)
    if(DEFINED ENV{XWIN_DIR})
        set(XWIN_DIR "$ENV{XWIN_DIR}")
    else()
        message(FATAL_ERROR "XWIN_DIR is not set (path to `xwin splat` output).")
    endif()
endif()

set(CMAKE_C_COMPILER clang-cl)
set(CMAKE_CXX_COMPILER clang-cl)
set(CMAKE_LINKER lld-link)
set(CMAKE_RC_COMPILER llvm-rc)
set(CMAKE_MT llvm-mt)
set(CMAKE_AR llvm-lib)

# Static-archive tools for clang-cl.
set(CMAKE_C_COMPILER_TARGET x86_64-pc-windows-msvc)
set(CMAKE_CXX_COMPILER_TARGET x86_64-pc-windows-msvc)

# System include roots (xwin lowercases directory names).
set(_xwin_incs
    "/imsvc${XWIN_DIR}/crt/include"
    "/imsvc${XWIN_DIR}/sdk/include/ucrt"
    "/imsvc${XWIN_DIR}/sdk/include/um"
    "/imsvc${XWIN_DIR}/sdk/include/shared"
    "/imsvc${XWIN_DIR}/sdk/include/winrt"
)
string(JOIN " " _xwin_incs_str ${_xwin_incs})

set(_common_flags
    "--target=x86_64-pc-windows-msvc \
     -Wno-unused-command-line-argument \
     -fms-compatibility -fms-extensions \
     -fdelayed-template-parsing \
     ${_xwin_incs_str}")

set(CMAKE_C_FLAGS_INIT "${_common_flags}")
set(CMAKE_CXX_FLAGS_INIT "${_common_flags}")

# Library search paths (xwin names the arch dir "x86_64").
set(_xwin_libs
    "-libpath:\"${XWIN_DIR}/crt/lib/x86_64\" \
     -libpath:\"${XWIN_DIR}/sdk/lib/ucrt/x86_64\" \
     -libpath:\"${XWIN_DIR}/sdk/lib/um/x86_64\"")

set(CMAKE_EXE_LINKER_FLAGS_INIT "${_xwin_libs}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${_xwin_libs}")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "${_xwin_libs}")

# Only search the target sysroot for libs/includes, host for programs.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

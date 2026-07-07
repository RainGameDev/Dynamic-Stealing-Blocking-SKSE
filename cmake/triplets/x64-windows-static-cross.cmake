# vcpkg triplet: Windows x64, fully static — static libraries AND a statically
# linked CRT (/MT). Cross-compiled from Linux via the clang-cl toolchain.
#
# We link the CRT statically so the finished plugin depends only on core Windows
# DLLs (kernel32/user32/...) and NOT on MSVCP140.dll / the UCRT api-sets. That
# makes it load reliably everywhere, including under Proton/Wine where the game's
# bundled C++ runtime otherwise fails to satisfy the plugin's imports (LoadLibrary
# error 193, ERROR_BAD_EXE_FORMAT).

set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE static)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Windows)

set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE
    "${CMAKE_CURRENT_LIST_DIR}/../cross/clang-cl-windows.cmake")

# Everything is C++; build all ports with this toolchain, release only.
set(VCPKG_BUILD_TYPE release)

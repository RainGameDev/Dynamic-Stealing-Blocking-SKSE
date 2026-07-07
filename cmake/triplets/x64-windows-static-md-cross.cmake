# vcpkg triplet: Windows x64, static libraries against the dynamic UCRT
# (x64-windows-static-md ABI, which is what CommonLibSSE-NG plugins use),
# cross-compiled from Linux via the clang-cl toolchain.

set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Windows)

set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE
    "${CMAKE_CURRENT_LIST_DIR}/../cross/clang-cl-windows.cmake")

# CommonLibSSE-NG and its deps are C++; let them all use this toolchain.
set(VCPKG_BUILD_TYPE release)

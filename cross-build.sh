#!/usr/bin/env bash
# Cross-compile the SKSE plugin to a Windows x64 DLL from Linux.
# Requires: clang-cl, lld-link, llvm-rc, ninja, a bootstrapped vcpkg, and an
# `xwin splat` output at ./.xwin (see README "Cross-compiling on Linux").
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export VCPKG_ROOT="${VCPKG_ROOT:-/home/rain/vcpkg}"
export XWIN_DIR="${XWIN_DIR:-$HERE/.xwin}"

echo ">> VCPKG_ROOT = $VCPKG_ROOT"
echo ">> XWIN_DIR   = $XWIN_DIR"

# xwin generates several case permutations of the SDK import libs, but occasionally
# misses the exact case a lib is referenced by (e.g. CommonLibSSE links "Version.lib").
# lld-link is case-sensitive on Linux, so add the missing symlink(s) if needed.
_umlib="$XWIN_DIR/sdk/lib/um/x86_64"
if [ -f "$_umlib/version.lib" ] && [ ! -e "$_umlib/Version.lib" ]; then
    ln -sf version.lib "$_umlib/Version.lib"
    echo ">> linked Version.lib -> version.lib"
fi

NINJA="$(command -v ninja)"

cmake -S "$HERE" -B "$HERE/build-cross" -G Ninja \
    -DCMAKE_MAKE_PROGRAM="$NINJA" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
    -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE="$HERE/cmake/cross/clang-cl-windows.cmake" \
    -DVCPKG_OVERLAY_TRIPLETS="$HERE/cmake/triplets" \
    -DVCPKG_TARGET_TRIPLET=x64-windows-static-md-cross \
    -DVCPKG_HOST_TRIPLET=x64-linux \
    -DVCPKG_INSTALL_OPTIONS="--allow-unsupported" \
    -DXWIN_DIR="$XWIN_DIR"

cmake --build "$HERE/build-cross" --config Release

echo
echo ">> Built:"
find "$HERE/build-cross" -name '*.dll' -printf '   %p (%s bytes)\n'

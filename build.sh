#!/bin/bash

set -e

COMPILER="gcc"
DO_CLEAN=0
DO_BUILD=0
CONFIG="release"

show_usage() {
    cat << EOF
Usage: ./build.sh [OPTIONS]

OPTIONS:
    --all                Build everything (clean + build)
    --clean              Clean build directory
    --build              Build the library
    --compiler COMPILER  Choose compiler: gcc or clang (default: gcc)
    --config CONFIG      Build configuration: debug or release (default: release)
    -h, --help           Show this help message

EXAMPLES:
    ./build.sh --all
    ./build.sh --clean --build
    ./build.sh --compiler clang --build
    ./build.sh --compiler gcc --config debug --all

EOF
}

while [[ $# -gt 0 ]]; do
    case $1 in
        --all)
            DO_CLEAN=1
            DO_BUILD=1
            shift
            ;;
        --clean)
            DO_CLEAN=1
            shift
            ;;
        --build)
            DO_BUILD=1
            shift
            ;;
        --compiler)
            COMPILER="$2"
            if [[ "$COMPILER" != "gcc" && "$COMPILER" != "clang" ]]; then
                echo "Error: Compiler must be 'gcc' or 'clang'"
                exit 1
            fi
            shift 2
            ;;
        --config)
            CONFIG="$2"
            if [[ "$CONFIG" != "debug" && "$CONFIG" != "release" ]]; then
                echo "Error: Config must be 'debug' or 'release'"
                exit 1
            fi
            shift 2
            ;;
        -h|--help)
            show_usage
            exit 0
            ;;
        *)
            echo "Error: Unknown option '$1'"
            show_usage
            exit 1
            ;;
    esac
done

if [[ $DO_CLEAN -eq 0 && $DO_BUILD -eq 0 ]]; then
    echo "Error: Must specify at least one action (--clean, --build, or --all)"
    show_usage
    exit 1
fi

if [[ "$COMPILER" == "clang" ]]; then
    export CC=clang
    export CXX=clang++
else
    export CC=gcc
    export CXX=g++
fi

echo "============================================"
echo "  Monte Carlo Options Library Build Script"
echo "============================================"
echo "Compiler: $COMPILER ($CC / $CXX)"
echo "Config:   $CONFIG"
echo ""

if [[ $DO_CLEAN -eq 1 ]]; then
    echo ">>> Cleaning..."
    rm -rf build/
    rm -rf obj/
    rm -f Makefile
    rm -f *.make
    echo "✓ Clean complete"
    echo ""
fi

if [[ $DO_BUILD -eq 1 ]]; then
    echo ">>> Generating build files with premake5..."
    premake5 gmake2
    echo ""
    
    echo ">>> Building library (config=$CONFIG)..."
    make config=$CONFIG verbose=1
    echo ""
    
    echo "✓ Build complete!"
    echo ""
    echo "Library location: build/libmcoptions.so"
    ls -lh build/libmcoptions.so 2>/dev/null || ls -lh build/libmcoptions.dylib 2>/dev/null || ls -lh build/mcoptions.dll 2>/dev/null
fi

echo ""
echo "============================================"
echo "  Build Summary"
echo "============================================"
echo "Actions performed:"
[[ $DO_CLEAN -eq 1 ]] && echo "  ✓ Clean"
[[ $DO_BUILD -eq 1 ]] && echo "  ✓ Build"
echo ""

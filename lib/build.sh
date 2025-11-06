#!/bin/bash

set -e

COMPILER="gcc"
DO_CLEAN=0
DO_BUILD=0
DO_TEST=0
TEST_FILE=""
CONFIG="release"
VENV_DIR=".venv"

show_usage() {
    cat << EOF
Usage: ./build.sh [OPTIONS]

OPTIONS:
    --all                Build everything (clean + build)
    --clean              Clean build directory and deactivate venv
    --build              Build the library
    --test [FILE]        Run tests (all or specific file)
    --compiler COMPILER  Choose compiler: gcc or clang (default: gcc)
    --config CONFIG      Build configuration: debug or release (default: release)
    -h, --help           Show this help message

TEST FILES:
    test_european        Run European option tests
    test_asian           Run Asian option tests
    test_american        Run American option tests
    test_variance_reduction  Run variance reduction tests

EXAMPLES:
    ./build.sh --all
    ./build.sh --clean --build
    ./build.sh --compiler clang --build
    ./build.sh --compiler gcc --config debug --all
    ./build.sh --build --test
    ./build.sh --test test_european
    ./build.sh --all --test

EOF
}

setup_venv() {
    if [[ ! -d "$VENV_DIR" ]]; then
        echo ">>> Creating virtual environment..."
        python3 -m venv "$VENV_DIR"
        echo "✓ Virtual environment created"
    fi
    
    echo ">>> Activating virtual environment..."
    source "$VENV_DIR/bin/activate"
    
    # Check and install dependencies
    echo ">>> Checking Python dependencies..."
    
    if ! python -c "import pytest" 2>/dev/null; then
        echo "  Installing pytest..."
        pip install -q pytest
    fi
    
    if ! python -c "import cffi" 2>/dev/null; then
        echo "  Installing cffi..."
        pip install -q cffi
    fi
 
    if ! python -c "import scipy" 2>/dev/null; then
        echo "  Installing scipy..."
        pip install -q scipy
    fi
 
    echo "✓ Dependencies ready"
}

cleanup_venv() {
    if [[ -n "$VIRTUAL_ENV" ]]; then
        echo ">>> Deactivating virtual environment..."
        deactivate 2>/dev/null || true
    fi
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
        --test)
            DO_TEST=1
            if [[ $# -gt 1 && ! "$2" =~ ^-- ]]; then
                TEST_FILE="$2"
                shift 2
            else
                shift
            fi
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

if [[ $DO_CLEAN -eq 0 && $DO_BUILD -eq 0 && $DO_TEST -eq 0 ]]; then
    echo "Error: Must specify at least one action (--clean, --build, --test, or --all)"
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
    cleanup_venv
    rm -rf build/
    rm -rf obj/
    rm -rf "$VENV_DIR"
    rm -f Makefile
    rm -f *.make
    rm -rf tests/__pycache__
    rm -rf .pytest_cache
    echo "✓ Clean complete"
    echo ""
fi

if [[ $DO_BUILD -eq 1 ]]; then
    echo ">>> Generating build files with premake5..."
    premake5 gmake2
    echo ""
    
    echo ">>> Building library (config=$CONFIG)..."
    make config=${CONFIG}_x64 verbose=1
    echo ""
    
    echo "✓ Build complete!"
    echo ""
    echo "Library location: build/libmcoptions.so"
    ls -lh build/libmcoptions.so 2>/dev/null || ls -lh build/libmcoptions.dylib 2>/dev/null || ls -lh build/mcoptions.dll 2>/dev/null
fi

if [[ $DO_TEST -eq 1 ]]; then
    echo ""
    echo "============================================"
    echo "  Running Tests"
    echo "============================================"
    
    # Check for Python
    if ! command -v python3 &> /dev/null; then
        echo "Error: python3 not found. Please install Python 3.7+"
        exit 1
    fi
    
    # Setup virtual environment and dependencies
    setup_venv
    
    # Check if library is built
    if [[ ! -f "build/libmcoptions.so" ]] && [[ ! -f "build/libmcoptions.dylib" ]] && [[ ! -f "build/mcoptions.dll" ]]; then
        echo "Error: Library not built. Run with --build first"
        cleanup_venv
        exit 1
    fi
    
    echo ""
    if [[ -z "$TEST_FILE" ]]; then
        echo ">>> Running all tests..."
        pytest tests/ -v --tb=short
    else
        echo ">>> Running tests/$TEST_FILE.py..."
        pytest tests/$TEST_FILE.py -v --tb=short
    fi
    
    TEST_EXIT_CODE=$?
    
    echo ""
    cleanup_venv
    
    if [[ $TEST_EXIT_CODE -ne 0 ]]; then
        exit $TEST_EXIT_CODE
    fi
fi

echo ""
echo "============================================"
echo "  Build Summary"
echo "============================================"
echo "Actions performed:"
[[ $DO_CLEAN -eq 1 ]] && echo "  ✓ Clean"
[[ $DO_BUILD -eq 1 ]] && echo "  ✓ Build"
[[ $DO_TEST -eq 1 ]] && echo "  ✓ Test"
echo ""

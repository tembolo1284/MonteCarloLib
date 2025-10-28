#!/bin/bash

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
DO_CLEAN=false
DO_BUILD=false
DO_RUN_SERVER=false
DO_RUN_CLIENT=false
DO_RUN_PYTHON=false
CONFIG="release"

print_header() {
    echo -e "${BLUE}============================================${NC}"
    echo -e "${BLUE}  Monte Carlo gRPC Server Build Script${NC}"
    echo -e "${BLUE}============================================${NC}"
    echo ""
}

print_usage() {
    echo "Usage: ./build.sh [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --clean              Clean build artifacts and generated files"
    echo "  --build              Build server and client"
    echo "  --run-server         Run the gRPC server"
    echo "  --run-client         Run the C++ client"
    echo "  --run-python         Run the Python client"
    echo "  --config <type>      Build configuration: debug or release (default: release)"
    echo "  --help               Show this help message"
    echo ""
    echo "Examples:"
    echo "  ./build.sh --clean --build"
    echo "  ./build.sh --build --run-server"
    echo "  ./build.sh --build --run-client"
    echo "  ./build.sh --clean --build --config debug"
    echo ""
    echo "Typical workflow:"
    echo "  Terminal 1: ./build.sh --build --run-server"
    echo "  Terminal 2: ./build.sh --run-client"
    echo "  Terminal 3: ./build.sh --run-python"
    echo ""
}

check_dependencies() {
    echo -e "${YELLOW}>>> Checking dependencies...${NC}"
    
    local missing=false
    
    if ! command -v protoc &> /dev/null; then
        echo -e "${RED}  ✗ protoc not found${NC}"
        missing=true
    else
        echo -e "${GREEN}  ✓ protoc found${NC}"
    fi
    
    if ! command -v grpc_cpp_plugin &> /dev/null; then
        echo -e "${RED}  ✗ grpc_cpp_plugin not found${NC}"
        missing=true
    else
        echo -e "${GREEN}  ✓ grpc_cpp_plugin found${NC}"
    fi
    
    if ! python3 -c "import grpc_tools" 2>/dev/null; then
        echo -e "${YELLOW}  ! Python grpcio-tools not found (optional for Python client)${NC}"
    else
        echo -e "${GREEN}  ✓ Python grpcio-tools found${NC}"
    fi
    
    if [ "$missing" = true ]; then
        echo ""
        echo -e "${RED}Missing required dependencies!${NC}"
        echo ""
        echo "Install with:"
        echo "  sudo apt install -y protobuf-compiler libprotobuf-dev"
        echo "  sudo apt install -y libgrpc++-dev libgrpc-dev protobuf-compiler-grpc"
        echo "  pip3 install grpcio grpcio-tools"
        echo ""
        exit 1
    fi
    
    echo ""
}

do_clean() {
    echo -e "${YELLOW}>>> Cleaning build artifacts...${NC}"
    
    # Remove build directory
    if [ -d "build" ]; then
        rm -rf build
        echo "  ✓ Removed build/"
    fi
    
    # Remove generated proto files
    if [ -d "generated" ]; then
        rm -rf generated
        echo "  ✓ Removed generated/"
    fi
    
    # Remove Makefile and make files
    rm -f Makefile *.make
    echo "  ✓ Removed Makefiles"
    
    echo -e "${GREEN}✓ Clean complete!${NC}"
    echo ""
}

generate_protos() {
    echo -e "${YELLOW}>>> Generating proto files...${NC}"
    
    # Create generated directory
    mkdir -p generated
    
    # Generate C++ files
    protoc \
        --cpp_out=generated \
        --grpc_out=generated \
        --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) \
        -I protos \
        protos/mcoptions.proto
    
    if [ $? -eq 0 ]; then
        echo "  ✓ C++ proto files generated"
    else
        echo -e "${RED}  ✗ Failed to generate C++ proto files${NC}"
        exit 1
    fi
    
    # Generate Python files
    python3 -m grpc_tools.protoc \
        -I protos \
        --python_out=generated \
        --grpc_python_out=generated \
        protos/mcoptions.proto
    
    if [ $? -eq 0 ]; then
        echo "  ✓ Python proto files generated"
    else
        echo -e "${YELLOW}  ! Failed to generate Python proto files (optional)${NC}"
    fi
    
    echo -e "${GREEN}✓ Proto generation complete${NC}"
    echo ""
}

do_build() {
    # Check dependencies first
    check_dependencies
    
    # Build the main library first
    echo -e "${YELLOW}>>> Building Monte Carlo library...${NC}"
    cd ../lib
    ./build.sh --build
    cd ../server
    echo -e "${GREEN}✓ Library built${NC}"
    echo ""
    
    # Generate proto files
    generate_protos
    
    # Generate build files
    echo -e "${YELLOW}>>> Generating build files...${NC}"
    premake5 gmake2
    echo -e "${GREEN}✓ Build files generated${NC}"
    echo ""
    
    # Determine the full config name (premake adds platform suffix)
    local make_config="${CONFIG}_x64"
    
    # Build
    echo -e "${YELLOW}>>> Building server and client...${NC}"
    echo -e "${BLUE}Compiler: gcc (gcc / g++)${NC}"
    echo -e "${BLUE}Config:   ${make_config}${NC}"
    make -j$(nproc) config=${make_config}
    echo -e "${GREEN}✓ Build complete!${NC}"
    echo ""
    
    # Show results
    echo -e "${GREEN}Build artifacts:${NC}"
    if [ -f "build/mcoptions_server" ]; then
        ls -lh build/mcoptions_server | awk '{print "  Server:    " $9 " (" $5 ")"}'
    fi
    if [ -f "build/mcoptions_client" ]; then
        ls -lh build/mcoptions_client | awk '{print "  Client:    " $9 " (" $5 ")"}'
    fi
    echo ""
    
    echo -e "${YELLOW}Next steps:${NC}"
    echo "  Run server: ./build.sh --run-server"
    echo "  Run client: ./build.sh --run-client"
    echo "  Run Python: ./build.sh --run-python"
    echo ""
}

do_run_server() {
    if [ ! -f "build/mcoptions_server" ]; then
        echo -e "${RED}Error: Server not built. Run with --build first.${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}============================================${NC}"
    echo -e "${GREEN}  Starting Monte Carlo Options Server${NC}"
    echo -e "${GREEN}============================================${NC}"
    echo ""
    echo -e "${YELLOW}Server will listen on 0.0.0.0:50051${NC}"
    echo ""
    echo -e "${YELLOW}To test the server (in separate terminals):${NC}"
    echo "  ./build.sh --run-client"
    echo "  ./build.sh --run-python"
    echo ""
    echo -e "${YELLOW}Press Ctrl+C to stop the server${NC}"
    echo ""
    
    # Set library path to include our library
    export LD_LIBRARY_PATH="$(pwd)/../lib/build:$LD_LIBRARY_PATH"
    ./build/mcoptions_server
}

do_run_client() {
    if [ ! -f "build/mcoptions_client" ]; then
        echo -e "${RED}Error: Client not built. Run with --build first.${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}============================================${NC}"
    echo -e "${GREEN}  Running C++ Client${NC}"
    echo -e "${GREEN}============================================${NC}"
    echo ""
    
    # Set library path to include our library
    export LD_LIBRARY_PATH="$(pwd)/../lib/build:$LD_LIBRARY_PATH"
    ./build/mcoptions_client
    
    echo ""
    echo -e "${GREEN}✓ Client test complete${NC}"
}

do_run_python() {
    if [ ! -d "generated" ]; then
        echo -e "${RED}Error: Proto files not generated. Run with --build first.${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}============================================${NC}"
    echo -e "${GREEN}  Running Python Client${NC}"
    echo -e "${GREEN}============================================${NC}"
    echo ""
    
    # Check if grpcio is installed
    if ! python3 -c "import grpc" 2>/dev/null; then
        echo -e "${YELLOW}Warning: grpcio not installed. Installing...${NC}"
        pip3 install grpcio grpcio-tools
        echo ""
    fi
    
    # Set PYTHONPATH to include generated files
    export PYTHONPATH="$(pwd)/generated:$PYTHONPATH"
    python3 client/python_client.py
    
    echo ""
    echo -e "${GREEN}✓ Python client test complete${NC}"
}

# Parse command line arguments
if [ $# -eq 0 ]; then
    print_header
    print_usage
    exit 0
fi

while [[ $# -gt 0 ]]; do
    case $1 in
        --clean)
            DO_CLEAN=true
            shift
            ;;
        --build)
            DO_BUILD=true
            shift
            ;;
        --run-server)
            DO_RUN_SERVER=true
            shift
            ;;
        --run-client)
            DO_RUN_CLIENT=true
            shift
            ;;
        --run-python)
            DO_RUN_PYTHON=true
            shift
            ;;
        --config)
            CONFIG="$2"
            if [ "$CONFIG" != "debug" ] && [ "$CONFIG" != "release" ]; then
                echo -e "${RED}Error: Config must be 'debug' or 'release'${NC}"
                exit 1
            fi
            shift 2
            ;;
        --help)
            print_header
            print_usage
            exit 0
            ;;
        *)
            echo -e "${RED}Error: Unknown option $1${NC}"
            echo ""
            print_usage
            exit 1
            ;;
    esac
done

# Execute actions
print_header

if [ "$DO_CLEAN" = true ]; then
    do_clean
fi

if [ "$DO_BUILD" = true ]; then
    do_build
fi

if [ "$DO_RUN_SERVER" = true ]; then
    do_run_server
fi

if [ "$DO_RUN_CLIENT" = true ]; then
    do_run_client
fi

if [ "$DO_RUN_PYTHON" = true ]; then
    do_run_python
fi

# If nothing was done, show help
if [ "$DO_CLEAN" = false ] && [ "$DO_BUILD" = false ] && [ "$DO_RUN_SERVER" = false ] && [ "$DO_RUN_CLIENT" = false ] && [ "$DO_RUN_PYTHON" = false ]; then
    print_usage
    exit 0
fi

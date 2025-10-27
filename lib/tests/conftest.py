import pytest
from cffi import FFI
import os

@pytest.fixture(scope="session")
def lib():
    """Load the shared library using CFFI"""
    ffi = FFI()
    
    # Parse the public C header
    header_path = os.path.join(os.path.dirname(__file__), "..", "include", "mcoptions.h")
    with open(header_path, 'r') as f:
        header = f.read()
    
    # Clean the header for CFFI
    cleaned_lines = []
    in_extern_c = False
    
    for line in header.split('\n'):
        stripped = line.strip()
        
        # Skip preprocessor directives
        if stripped.startswith('#'):
            continue
        
        # Skip extern "C" opening
        if 'extern "C"' in stripped and '{' in stripped:
            in_extern_c = True
            continue
        
        # Skip the closing brace of extern "C"
        if in_extern_c and stripped == '}':
            in_extern_c = False
            continue
        
        # Remove MCO_API macro
        line = line.replace('MCO_API', '')
        
        cleaned_lines.append(line)
    
    header = '\n'.join(cleaned_lines)
    
    # Parse with CFFI
    ffi.cdef(header)
    
    # Load the compiled shared library
    lib_path = os.path.join(os.path.dirname(__file__), "..", "build", "libmcoptions.so")
    if not os.path.exists(lib_path):
        lib_path = lib_path.replace(".so", ".dylib")  # macOS
    if not os.path.exists(lib_path):
        lib_path = lib_path.replace(".dylib", ".dll")  # Windows
    
    if not os.path.exists(lib_path):
        pytest.skip("Library not built. Run ./build.sh --all first")
    
    return ffi, ffi.dlopen(lib_path)

@pytest.fixture
def ctx(lib):
    """Create a fresh context for each test"""
    ffi, mco = lib
    context = mco.mco_context_new()
    mco.mco_context_set_seed(context, 42)
    yield (ffi, mco, context)
    mco.mco_context_free(context)

workspace "mcoptions_grpc"
    configurations { "Debug", "Release" }
    platforms { "x64" }
    language "C++"
    cppdialect "C++17"
    
    filter "configurations:Debug"
        symbols "On"
        optimize "Off"
        
    filter "configurations:Release"
        optimize "Speed"
        symbols "Off"

-- Helper function to generate proto files
newaction {
    trigger = "proto",
    description = "Generate protobuf and gRPC code",
    execute = function()
        os.mkdir("generated")
        
        -- Generate C++ files
        os.execute("protoc --cpp_out=generated --grpc_out=generated --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) -I protos protos/mcoptions.proto")
        
        -- Generate Python files
        os.execute("python3 -m grpc_tools.protoc -I protos --python_out=generated --grpc_python_out=generated protos/mcoptions.proto")
        
        print("✓ Proto files generated in generated/")
    end
}

project "mcoptions_server"
    kind "ConsoleApp"
    targetdir "build"
    objdir "build/obj"
    
    files {
        "server/main.cpp",
        "server/mcoptions_service.hpp",
        "generated/mcoptions.pb.cc",
        "generated/mcoptions.grpc.pb.cc"
    }
    
    includedirs {
        "generated",
        "../lib/include",
        "/usr/include",
        "/usr/local/include"
    }
    
    libdirs {
        "../lib/build",
        "/usr/lib",
        "/usr/local/lib",
        "/usr/lib/x86_64-linux-gnu"
    }
    
    links {
        "mcoptions",
        "grpc++",
        "grpc",
        "protobuf",
        "pthread",
        "dl"
    }
    
    filter "system:linux"
        links { "m" }

project "mcoptions_client"
    kind "ConsoleApp"
    targetdir "build"
    objdir "build/obj"
    
    files {
        "client/cpp_client.cpp",
        "generated/mcoptions.pb.cc",
        "generated/mcoptions.grpc.pb.cc"
    }
    
    includedirs {
        "generated",
        "/usr/include",
        "/usr/local/include"
    }
    
    libdirs {
        "/usr/lib",
        "/usr/local/lib",
        "/usr/lib/x86_64-linux-gnu"
    }
    
    links {
        "grpc++",
        "grpc",
        "protobuf",
        "pthread",
        "dl"
    }

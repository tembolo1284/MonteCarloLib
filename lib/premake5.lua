workspace "mcoptions"
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

project "mcoptions"
    kind "SharedLib"
    targetdir "build"
    objdir "build/obj"
    
    files {
        -- Core
        "src/api.cpp",
        "src/context.cpp",
        
        -- Instruments
        "src/instruments/**.cpp",
        "include/internal/instruments/**.hpp",
        
        -- Models
        "src/models/**.cpp",
        "include/internal/models/**.hpp",
        
        -- Methods
        "src/methods/**.cpp",
        "include/internal/methods/**.hpp",
        
        -- Variance Reduction (when we add them)
        "src/variance_reduction/**.cpp",
        "include/internal/variance_reduction/**.hpp",
        
        -- Public headers
        "include/mcoptions.h",
        
        -- Core utilities
        "include/internal/context.hpp",
        "include/internal/random.hpp"
    }
    
    includedirs {
        "include"
    }
    
    defines { "MCOPTIONS_EXPORTS" }
    
    filter "system:linux"
        links { "m" }
        buildoptions { "-fPIC" }
    
    filter "system:windows"
        defines { "_CRT_SECURE_NO_WARNINGS" }

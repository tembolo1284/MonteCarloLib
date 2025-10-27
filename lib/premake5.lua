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
        "src/**.cpp",
        "include/mcoptions.h",
        "include/internal/**.hpp"
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

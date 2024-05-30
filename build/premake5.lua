workspace "XBR"
    configurations { "Development", "Debug", "Release" }
    language "C++"
    cppdialect "C++20"
    conformancemode "Off"
    pchheader "pch.h"
    pchsource "pch.cpp"
    platforms { "Win64" }
    targetdir "bin/%{cfg.buildcfg}"
    objdir "bin/obj/%{cfg.buildcfg}"
    files { "src/**.h", "src/Stl/**", "src/pch.cpp" }
    justmycode "Off"
    pchheader "pch.h"
    pchsource "pch.cpp"

    filter "platforms:Win64"
        system "Windows"
        architecture "x86_64"
        defines { "XBR_WIN64" }
        links { "Kernel32", "Shell32", "User32", "Gdi32", "dxgi", "d3d12", "d3dcompiler", "dxguid" }

    filter "configurations:Development"
        defines { "_DEVEL", "_DEBUG" }
        symbols "On"

    filter "configurations:Debug"
        defines { "_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

project "Engine"
    kind "StaticLib"
    defines { "XBR_ENGINE_LIB" }
    files { "src/Engine/**" }
    includedirs { "src", "vendor" }

project "Editor"
    kind "ConsoleApp"
    defines { "Editor" }
    files { "src/Editor/**" }
    dependson { "Engine" }
    links { "Engine" }
    includedirs { "src", "vendor" }
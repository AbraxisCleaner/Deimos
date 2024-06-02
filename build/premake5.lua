workspace "Talos"
    configurations { "Development", "Debug", "Release" }
    language "C++"
    cppdialect "C++17"
    conformancemode "Off"
    pchheader "pch.h"
    pchsource "pch.cpp"
    platforms { "Win64" }
    targetdir "bin/%{cfg.buildcfg}"
    objdir "bin/obj/%{cfg.buildcfg}"
    files { "source/**.h", "source/Stl/**", "source/pch.cpp" }
    justmycode "Off"

    filter "platforms:Win64"
        system "Windows"
        architecture "x86_64"
        defines { "ENGINE_WIN64" }
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

project "Editor"
    kind "ConsoleApp"
    files { "source/Engine/**", "source/Editor/**" }
    includedirs { "source", "source/vendor" }
    pchheader "pch.h"
    pchsource "pch.cpp"
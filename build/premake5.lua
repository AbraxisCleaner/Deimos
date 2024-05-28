workspace "XBR"
    configurations { "Development", "Debug", "Release" }
    language "C++"
    cppdialect "C++20"
    conformancemode "Off"
    platforms { "Win64" }
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
    pchheader "pch.h"
    pchsource "pch.cpp"
    targetdir "bin/%{cfg.buildcfg}"

project "Editor"
    kind "ConsoleApp"
    defines { "XBR_EDITOR" }
    files { "src/**.h", "src/Editor/**", "src/pch*" }
    includedirs { "src", "src/Editor" }


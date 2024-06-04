workspace "Talos"
    configurations { "Development", "Debug", "Release" }
    language "C++"
    cppdialect "C++17"
    conformancemode "Off"
    justmycode "Off"
    pchheader "pch.h"
    pchsource "source/pch.cpp"
    platforms { "Win64" }
    targetdir "bin/%{cfg.buildcfg}"
    objdir "bin/obj/%{cfg.buildcfg}"
    files { "source/**.h", "source/Stl/**", "source/pch.cpp", "source/vendor/volk.c" }
    includedirs { "source", "source/vendor" }
    libdirs { "source/vendor" }

    filter "platforms:Win64"
        system "Windows"
        architecture "x86_64"
        defines { "PLATFORM_WIN64", "VK_USE_PLATFORM_WIN32_KHR" }
        links { "Kernel32", "Shell32", "User32", "Gdi32", "vulkan-1.lib" }

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
    files { "source/Engine/**", "source/Editor/**" }
    pchheader "pch.h"
    filter "configurations:Development"
        kind "ConsoleApp"
    filter "configurations:Debug"
        kind "ConsoleApp"
    filter "configurations:Release"
        kind "WindowedApp"
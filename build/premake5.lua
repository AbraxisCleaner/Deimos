workspace "Solution"
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
    files { "source/**.h", "source/Stl/**", "source/pch.cpp" }
    includedirs { "source", "source/vendor" }
    libdirs { "source/vendor", "bin/%{cfg.buildcfg}" }

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

project "Engine"
    files { "source/Engine/Private/*.cpp" }
    pchheader "pch.h"
    kind "StaticLib"

project "Editor"
    dependson { "Engine", "RhiVulkan" }
    files { "source/Editor/Private/*.cpp" }
    pchheader "pch.h"
    kind "ConsoleApp"

project "Game"
    dependson { "RhiVulkan", "Engine" }
    links { "Engine" }
    files { "source/Game/Private/*.cpp" }
    pchheader "pch.h"
    kind "ConsoleApp"
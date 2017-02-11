workspace "LabCmd"

configurations { "Debug", "Release" }
architecture "x86_64"

--objdir ("../build/obj/%{cfg.longname}/%{prj.name}")

platforms {
        "linux",
        "macosx",
        "windows"
    }

filter "system:linux"
    system "linux"
    defines { "PLATFORM_LINUX" }
    buildoptions { "-std=c++11" }

filter "system:windows"
    system "windows"
    defines {  "PLATFORM_WINDOWS", "_CRT_SECURE_NO_WARNINGS" }
    characterset ("MBCS")

filter "system:macosx"
    system "macosx"
    defines {  "PLATFORM_DARWIN" }

filter {}

filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"

filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"

filter {}

project "LabCmd"
    kind "SharedLib"
    language "C++"

    -- targetdir ("local/lib/%{cfg.longname}")

    defines { "BUILDING_LABCMD" }

    includedirs {
        "src"
    }
    files {
         "include/**.h", "src/**.cpp", "src/**.c"
    }
    excludes { }
    libdirs {
        "../../local/lib"
    }
    links { }


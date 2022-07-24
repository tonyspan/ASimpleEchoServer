workspace "EchoServer"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "NetLibrary"
	location "NetLibrary"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src"
	}

	libdirs
	{
	}

	postbuildcommands
	{
	}

	filter "system:windows"
		systemversion "latest"
		links
		{
			"Ws2_32",
			"Mswsock",
			"AdvApi32"
		}
		
		excludes
		{
			"%{prj.name}/src/Platform/Linux/**.h",
			"%{prj.name}/src/Platform/Linux/**.cpp"
		}

	filter "system:linux"
		systemversion "latest"
		links
		{
			"pthread"
		}

		excludes
		{
			"%{prj.name}/src/Platform/Windows/**.h",
			"%{prj.name}/src/Platform/Windows/**.cpp"
		}

	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		optimize "On"

project "Server"
	location "Server"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"NetLibrary/src"
	}

	libdirs
	{
	}

	links
	{
		"NetLibrary"
	}

	postbuildcommands
	{
	}

	filter "system:windows"
		systemversion "latest"
	
	filter "system:linux"
		systemversion "latest"
		links
		{
			"pthread"
		}

	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		optimize "On"


project "Client"
	location "Client"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"NetLibrary/src"
	}

	libdirs
	{
	}

	links
	{
		"NetLibrary"
	}

	postbuildcommands
	{
	}

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		systemversion "latest"
		links
		{
			"pthread"
		}

	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		optimize "On"

function rw_filter_windows()
	filter({ 'system:' .. build_platforms.Windows })
end

function rw_filter_linux()
	filter({ 'system:' .. build_platforms.Linux })
end

function rw_filter_configuration(configuration)
	filter({ 'configurations:' .. configuration })
end

function rw_filter_configuration_debug()
	rw_filter_configuration(build_configurations.Debug)
end

function rw_filter_configuration_development()
	rw_filter_configuration(build_configurations.Development)
end

function rw_filter_configuration_shipping()
	rw_filter_configuration(build_configurations.Shipping)
end

function rw_filter_visual_studio()
	filter({ 'action:vs2022' })
end

function rw_filter_end()
	filter({})
end

function rw_enable_multi_processor_compilation()
	rw_filter_visual_studio()
		flags({
			'MultiProcessorCompile',
		})
	rw_filter_end()
end

function rw_default_compiler_flags()
	characterset('Unicode')
	callingconvention('FastCall')
	floatingpoint('Fast') -- If issues arise, change this
	rw_enable_multi_processor_compilation()
end

function rw_set_project_output()
	targetdir(path.join(project_build_output_path, '%{prj.name}'))
	objdir(path.join(project_intermediate_output_path, '%{prj.name}'))
end

function rw_configuration_flags(configuration)
	if configuration == build_configurations.Debug then
		runtime('Debug')
		symbols('On')
		optimize('Off')
		defines({
			macro_prefix .. 'CONFIG_DEBUG=1',
			macro_prefix .. 'CONFIG=TEXT("Debug")',
			'_DEBUG=1',
			macro_prefix .. 'CONFIG_DEVELOPMENT=0',
			macro_prefix .. 'CONFIG_SHIPPING=0',
		})
	elseif configuration == build_configurations.Development then
		runtime('Debug')
		symbols('On')
		optimize('Debug')
		defines({
			macro_prefix .. 'CONFIG_DEVELOPMENT=1',
			macro_prefix .. 'CONFIG=TEXT("Development")',
			'_DEBUG=1',
		})
	elseif configuration == build_configurations.Shipping then
		runtime('Release')
		symbols('Off')
		optimize('Full')
		defines({
			macro_prefix .. 'CONFIG_SHIPPING=1',
			macro_prefix .. 'CONFIG=TEXT("Shipping")',
			'NDEBUG=1',
		})
	end
end

function rw_platform_flags(platform)
	if platform == build_platforms.Windows then
		defines({
			macro_prefix .. 'PLATFORM_WINDOWS=1',
			macro_prefix .. 'PLATFORM=TEXT("Windows")',
			'WIN32_LEAN_AND_MEAN',
			'NOMINMAX',
			macro_prefix .. 'PLATFORM_LINUX=0',
		})
		disablewarnings({
			'4251',
		})
	elseif platform == build_platforms.Linux then
		defines({
			macro_prefix .. 'PLATFORM_LINUX=1',
			macro_prefix .. 'PLATFORM=TEXT("Linux")',
			macro_prefix .. 'PLATFORM_WINDOWS=0',
		})
	end
end

function rw_project(name)
	project(name)
		rw_default_compiler_flags()
		rw_set_project_output()

		rw_filter_configuration_debug()
			rw_configuration_flags(build_configurations.Debug)
		rw_filter_end()

		rw_filter_configuration_development()
			rw_configuration_flags(build_configurations.Development)
		rw_filter_end()

		rw_filter_configuration_shipping()
			rw_configuration_flags(build_configurations.Shipping)
		rw_filter_end()

		rw_filter_windows()
			rw_platform_flags(build_platforms.Windows)
		rw_filter_end()

		rw_filter_linux()
			rw_platform_flags(build_platforms.Linux)
		rw_filter_end()
end

function rw_default_files()
	files({
		path.join(source_folder_path, '%{prj.name}', '**.h'),
		path.join(source_folder_path, '%{prj.name}', '**.hpp'),
		path.join(source_folder_path, '%{prj.name}', '**.cpp'),
	})
end

function rw_default_includes()
	includedirs({
		path.join(source_folder_path, '%{prj.name}/Public'),
		path.join(source_folder_path, '%{prj.name}/Private'),
	})
end

function rw_language_cpp()
	language('C++')
	cppdialect(cpp_standard)
end

function rw_precompiled_header()
	files {
	  '%{prj.location}/pch.cpp',
	  '%{prj.location}/pch.h',
	}
	pchheader('pch.h')
	pchsource '%{prj.location}/pch.cpp'
end

function rw_kind_console_app(name)
	kind('ConsoleApp')
	defines({
		macro_prefix .. 'KIND_CONSOLE_APP=1',
		macro_prefix .. 'KIND=TEXT("ConsoleApp")',
		macro_prefix .. 'KIND_WINDOWED_APP=0',
		macro_prefix .. 'KIND_STATIC_LIB=0',
		macro_prefix .. 'KIND_SHARED_LIB=0',
		macro_prefix .. 'COMPILE_' .. string.upper(name) .. '_API=0',
	})
end

function rw_kind_windowed_app(name)
	kind('WindowedApp')
	defines({
		macro_prefix .. 'KIND_WINDOWED_APP=1',
		macro_prefix .. 'KIND=TEXT("WindowedApp")',
		macro_prefix .. 'KIND_CONSOLE_APP=0',
		macro_prefix .. 'KIND_STATIC_LIB=0',
		macro_prefix .. 'KIND_SHARED_LIB=0',
		macro_prefix .. 'COMPILE_' .. string.upper(name) .. '_API=0',
	})
end

function rw_kind_static_lib(name)
	kind('StaticLib')
	defines({
		macro_prefix .. 'KIND_STATIC_LIB=1',
		macro_prefix .. 'KIND=TEXT("StaticLib")',
		macro_prefix .. 'COMPILE_' .. string.upper(name) .. '_API=1',
		macro_prefix .. 'KIND_CONSOLE_APP=0',
		macro_prefix .. 'KIND_WINDOWED_APP=0',
		macro_prefix .. 'KIND_SHARED_LIB=0',
	})
end

function rw_kind_shared_lib(name)
	kind('SharedLib')
	defines({
		macro_prefix .. 'KIND_SHARED_LIB=1',
		macro_prefix .. 'KIND=TEXT("SharedLib")',
		macro_prefix .. 'COMPILE_' .. string.upper(name) .. '_API=1',
		macro_prefix .. 'KIND_CONSOLE_APP=0',
		macro_prefix .. 'KIND_STATIC_LIB=0',
		macro_prefix .. 'KIND_SHARED_LIB=0',
	})
end

function rw_default_location()
	location(path.join(source_folder_path, '%{prj.name}'))
end

function rw_third_party_location()
	location(path.join(third_party_folder_path, '%{prj.name}'))
end

function rw_link_project(name, include_path)
	if not include_path then
		include_path = source_folder_path .. '/' .. name .. '/Public'
	end
	includedirs ({
		include_path,
	})
	links ({
		name
	})
end

function rw_copy_output_to_directory(directory)
	prebuildcommands ({
		('{MKDIR} "' .. directory .. '"')
	})
	postbuildcommands ({
		('{COPY} %{cfg.buildtarget.relpath} "' .. directory .. '"')
	})
end

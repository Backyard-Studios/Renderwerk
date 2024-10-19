workspace(project_name)
	location(root_path)

	configurations({
		build_configurations.Debug,
		build_configurations.Development,
		build_configurations.Shipping,
	})

	platforms({
		build_platforms.Windows,
	})

	defaultplatform(build_platforms.Windows)

	rw_filter_windows()
		system('windows')
		architecture('x86_64')
	rw_filter_end()

	defines({
		'RW_ENGINE_NAME="' .. project_name .. '"',
		'RW_ENGINE_VERSION="' .. project_version .. '"',
		'RW_ENGINE_VERSION_MAJOR=' .. project_version_major,
		'RW_ENGINE_VERSION_MINOR=' .. project_version_minor,
		'RW_ENGINE_VERSION_PATCH=' .. project_version_patch,
		'RW_ENGINE_VERSION_SUFFIX="' .. project_version_suffix .. '"',
		'RW_ENGINE_FULL_VERSION="' .. project_version_string .. '"',
		'RW_ENGINE_AUTHOR="' .. project_author .. '"',
		'RW_LIBRARY_SHARED=1',
		'SPDLOG_USE_STD_FORMAT=1',
		'SPDLOG_WCHAR_FILENAMES=1',
		'SPDLOG_WCHAR_TO_UTF8_SUPPORT=1',
	})

	rw_filter_configuration_debug()
		defines({
			'RW_ENABLE_MEMORY_TRACKING=1',
			'SPDLOG_ACTIVE_LEVEL=0',
		})
	rw_filter_end()
	
	rw_filter_configuration_development()
		defines({
			'RW_ENABLE_MEMORY_TRACKING=1',
			'SPDLOG_ACTIVE_LEVEL=1',
		})
	rw_filter_end()
	
	rw_filter_configuration_shipping()
		defines({
			'RW_ENABLE_MEMORY_TRACKING=0',
			'SPDLOG_ACTIVE_LEVEL=3',
		})
	rw_filter_end()

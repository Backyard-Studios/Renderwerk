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
	})

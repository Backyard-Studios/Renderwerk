rw_project('Editor')
	rw_default_location()
	rw_language_cpp()

	rw_filter_configuration_debug()
		rw_kind_console_app('Editor')
	rw_filter_end()

	rw_filter_configuration_development()
		rw_kind_console_app('Editor')
	rw_filter_end()

	rw_filter_configuration_shipping()
		rw_kind_windowed_app('Editor')
	rw_filter_end()

	rw_default_files()
	rw_default_includes()
	rw_precompiled_header()

	-- Dependencies
	rw_link_project('Engine')
	rw_link_project('spdlog', rw_make_third_party_location(path.join('spdlog', 'include')))
	rw_link_project('tracy', rw_make_third_party_location(path.join('tracy', 'public')))
	rw_link_d3d12()

	rw_copy_d3d12_binaries()

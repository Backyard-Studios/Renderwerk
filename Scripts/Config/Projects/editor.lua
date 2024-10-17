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

	rw_link_project('Engine')
	
	rw_copy_assets()

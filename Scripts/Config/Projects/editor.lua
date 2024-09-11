rw_project('Editor')
	rw_default_location()
	rw_language_cpp()
	rw_kind_console_app('Editor')

	rw_default_files()
	rw_default_includes()
	rw_precompiled_header()

	rw_link_project('Engine')

rw_project('EngineTests')
	rw_default_location()
	rw_language_cpp()
	rw_kind_console_app('EngineTests')

	rw_default_files()
	rw_default_includes()

	rw_link_project('Engine')

	rw_link_project('googletest', path.join(third_party_folder_path, 'googletest', 'googletest', 'include'))
	rw_link_project('googlemock', path.join(third_party_folder_path, 'googletest', 'googlemock', 'include'))

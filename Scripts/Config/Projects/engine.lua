rw_project('Engine')
	rw_default_location()
	rw_language_cpp()
	rw_kind_shared_lib('Engine')

	rw_default_files()
	rw_default_includes()
	rw_precompiled_header()

	rw_copy_output_to_directory(path.join(project_build_output_path, 'Editor'))
	rw_copy_output_to_directory(path.join(project_build_output_path, 'EngineTests'))

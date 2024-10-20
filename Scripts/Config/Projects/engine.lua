rw_project('Engine')
	rw_default_location()
	rw_language_cpp()
	rw_kind_shared_lib('Engine')

	rw_default_files()
	rw_default_includes()
	rw_precompiled_header()

	defines({
		'RW_COMPILE_LIBRARY=1',
	})

	rw_copy_output_to_directory(path.join(project_build_output_path, 'Editor'))

	-- Dependencies
	rw_link_project('spdlog', rw_make_third_party_location(path.join('spdlog', 'include')))
	rw_link_project('tracy', rw_make_third_party_location(path.join('tracy', 'public')))

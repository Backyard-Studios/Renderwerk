rw_project('Engine')
	rw_default_location()
	rw_language_cpp()
	rw_kind_shared_lib('Engine')

	rw_default_files()
	rw_default_includes()
	rw_precompiled_header()

	rw_copy_output_to_directory(path.join(project_build_output_path, 'Editor'))
	rw_copy_output_to_directory(path.join(project_build_output_path, 'EngineTests'))

	rw_link_project('tracy', rw_make_third_party_location(path.join('tracy', 'public')))
	rw_link_project('spdlog', rw_make_third_party_location(path.join('spdlog', 'include')))
	rw_link_project('flecs', rw_make_third_party_location(path.join('flecs', 'include')))
	rw_link_project('imgui', rw_make_third_party_location(path.join('imgui')))

	defines({
		'TRACY_ENABLE',
		'TRACY_ON_DEMAND',
	})

	rw_link_d3d12()

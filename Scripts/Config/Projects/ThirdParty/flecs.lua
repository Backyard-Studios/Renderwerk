rw_project('flecs')
	rw_third_party_location()
	rw_language_cpp()
	rw_kind_static_lib('flecs')

	files({
		rw_make_third_party_project_location(path.join('include', '**.h')),
		rw_make_third_party_project_location(path.join('src', '**.h')),
		rw_make_third_party_project_location(path.join('src', '**.c')),
	})

	includedirs({
		rw_make_third_party_project_location('include'),
		rw_make_third_party_project_location('src'),
	})

	defines({
		'flecs_STATIC=1',
	})

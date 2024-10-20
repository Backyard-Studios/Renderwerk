rw_project('spdlog')
	rw_third_party_location()
	rw_language_cpp()
	rw_kind_static_lib('spdlog')

	files({
		rw_make_third_party_project_location(path.join('include', '**.h')),
		rw_make_third_party_project_location(path.join('src', '**.cpp')),
	})

	includedirs({
		rw_make_third_party_project_location('include'),
	})

	defines({
		'SPDLOG_COMPILED_LIB=1',
	})

	rw_filter_windows()
		disablewarnings({
			'4006',
		})
		linkoptions({
			'/IGNORE:4006',
		})
	rw_filter_end()

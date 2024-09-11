rw_project('googletest')
	rw_third_party_location()
	rw_language_cpp()
	rw_kind_static_lib('googletest')

	files({
		rw_make_third_party_project_location(path.join('googletest', 'src', '**.cc')),
		rw_make_third_party_project_location(path.join('googletest', 'src', '*.h')),
		rw_make_third_party_project_location(path.join('googletest', 'include', '*.h')),
	})

	includedirs({
		rw_make_third_party_project_location(path.join('googletest')),
		rw_make_third_party_project_location(path.join('googletest', 'include')),
	})

	rw_filter_windows()
		disablewarnings({
			'4006',
		})
		linkoptions({
			'/IGNORE:4006',
		})
	rw_filter_end()

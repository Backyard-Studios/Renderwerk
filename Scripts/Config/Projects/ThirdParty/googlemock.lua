rw_project('googlemock')
	rw_third_party_location()
	rw_language_cpp()
	rw_kind_static_lib('googlemock')

	files({
		rw_make_third_party_location(path.join('googletest', 'googlemock', 'src', '**.cc')),
		rw_make_third_party_location(path.join('googletest', 'googlemock', 'include', '**.h')),
	})

	includedirs({
		rw_make_third_party_location(path.join('googletest', 'googlemock')),
		rw_make_third_party_location(path.join('googletest', 'googlemock', 'include')),
	})

	rw_filter_windows()
		disablewarnings({
			'4006',
		})
		linkoptions({
			'/IGNORE:4006',
		})
	rw_filter_end()

	rw_link_project('googletest', path.join(third_party_folder_path, 'googletest', 'googletest', 'include'))

rw_project('tracy')
	rw_third_party_location()
	language('C++')
	cppdialect('C++17')
	rw_kind_static_lib('tracy')

	files({
		rw_make_third_party_project_location(path.join('public', 'client', '**.h')),
		rw_make_third_party_project_location(path.join('public', 'client', '**.hpp')),
		rw_make_third_party_project_location(path.join('public', 'client', '**.cpp')),
		rw_make_third_party_project_location(path.join('public', 'common', '**.h')),
		rw_make_third_party_project_location(path.join('public', 'common', '**.hpp')),
		rw_make_third_party_project_location(path.join('public', 'common', '**.cpp')),
		rw_make_third_party_project_location(path.join('public', 'tracy', '**.h')),
		rw_make_third_party_project_location(path.join('public', 'tracy', '**.hpp')),
		rw_make_third_party_project_location(path.join('public', 'tracy', '**.cpp')),
		rw_make_third_party_project_location(path.join('public', 'libbacktrace', '**.hpp')),
		rw_make_third_party_project_location(path.join('public', 'libbacktrace', 'alloc.cpp')),
		rw_make_third_party_project_location(path.join('public', 'libbacktrace', 'sort.cpp')),
		rw_make_third_party_project_location(path.join('public', 'libbacktrace', 'state.cpp')),
		rw_make_third_party_project_location(path.join('public', 'TracyClient.cpp')),
	})

	includedirs({
		rw_make_third_party_project_location('public'),
	})

	rw_filter_linux()
		files({
			rw_make_third_party_project_location(path.join('public', 'libbacktrace', 'posix.cpp')),
			rw_make_third_party_project_location(path.join('public', 'libbacktrace', 'mmapio.cpp')),
			rw_make_third_party_project_location(path.join('public', 'libbacktrace', 'macho.cpp')),
			rw_make_third_party_project_location(path.join('public', 'libbacktrace', 'fileline.cpp')),
			rw_make_third_party_project_location(path.join('public', 'libbacktrace', 'elf.cpp')),
			rw_make_third_party_project_location(path.join('public', 'libbacktrace', 'dwarf.cpp')),
		})
	rw_filter_end()

	defines({
		'TRACY_ENABLE',
		'TRACY_ON_DEMAND',
	})

	links({
		'dbghelp.lib'
	})

	rw_filter_windows()
		disablewarnings({
			'4996',
		})
		linkoptions({
			'/IGNORE:4996',
		})
	rw_filter_end()

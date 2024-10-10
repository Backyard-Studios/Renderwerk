rw_project('imgui')
	rw_third_party_location()
	rw_language_cpp()
	rw_kind_static_lib('imgui')

	files({
		rw_make_third_party_project_location(path.join('imconfig.h')),
		rw_make_third_party_project_location(path.join('imgui_internal.h')),
		rw_make_third_party_project_location(path.join('imgui.h')),
		rw_make_third_party_project_location(path.join('imstb_rectpack.h')),
		rw_make_third_party_project_location(path.join('imstb_textedit.h')),
		rw_make_third_party_project_location(path.join('imstb_truetype.h')),
		rw_make_third_party_project_location(path.join('backends', 'imgui_impl_dx12.h')),
		rw_make_third_party_project_location(path.join('backends', 'imgui_impl_win32.h')),
		rw_make_third_party_project_location(path.join('imgui_demo.cpp')),
		rw_make_third_party_project_location(path.join('imgui_draw.cpp')),
		rw_make_third_party_project_location(path.join('imgui_tables.cpp')),
		rw_make_third_party_project_location(path.join('imgui_widgets.cpp')),
		rw_make_third_party_project_location(path.join('imgui.cpp')),
		rw_make_third_party_project_location(path.join('backends', 'imgui_impl_dx12.cpp')),
		rw_make_third_party_project_location(path.join('backends', 'imgui_impl_win32.cpp')),
	})

	includedirs({
		rw_make_third_party_project_location(),
	})

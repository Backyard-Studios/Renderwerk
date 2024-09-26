rw_project('D3D12MemoryAllocator')
	rw_third_party_location()
	rw_language_cpp()
	rw_kind_static_lib('D3D12MemoryAllocator')

	files({
		rw_make_third_party_project_location(path.join('include', 'D3D12MemAlloc.h')),
		rw_make_third_party_project_location(path.join('src', 'D3D12MemAlloc.cpp')),
	})

	includedirs({
		rw_make_third_party_project_location('include'),
	})

include('properties_preprocessed.lua') -- This file is generated by the setup script
include('utility.lua')

include('workspace.lua')

group('Engine')
	include('Projects/engine.lua')

group('Applications')
	include('Projects/editor.lua')

group('ThirdParty')
	include('Projects/ThirdParty/spdlog.lua')
	include('Projects/ThirdParty/tracy.lua')

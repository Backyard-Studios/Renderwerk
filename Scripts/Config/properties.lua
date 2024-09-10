-- []=========================================================================[]
-- Note: This file is used to define properties that are used by the build
-- system. Please DO NOT modify the properties_preprocessed.lua file directly.
-- Instead modify this file and run the setup script to regenerate it.
--
-- You can use the following variables in this file:
--   - $(PROJECT_NAME) - The name of the project.
--   - $(PROJECT_NAME_SHORTHAND) - The shorthand uppercase name of the project.
--   - $(PROJECT_VERSION) - The version of the project.
--   - $(PROJECT_VERSION_SUFFIX) - The version suffix of the project.
--   - $(PROJECT_VERSION_STRING) - The version string of the project.
--   - $(PROJECT_AUTHOR) - The author of the project.
--   - $(ROOT_PATH) - The root path of the project.
-- []=========================================================================[]

project_name = '$(PROJECT_NAME)'
project_version = '$(PROJECT_VERSION)'
project_version_suffix = '$(PROJECT_VERSION_SUFFIX)'
project_version_string = '$(PROJECT_VERSION_STRING)'
project_author = '$(PROJECT_AUTHOR)'

root_path = path.getabsolute('$(ROOT_PATH)')

build_configurations = {
	Debug = 'Debug',
	Development = 'Development',
	Shipping = 'Shipping',
}

build_platforms = {
	Windows = 'Windows',
	Linux = 'Linux',
}

macro_prefix = '$(PROJECT_NAME_SHORTHAND)_'

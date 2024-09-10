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

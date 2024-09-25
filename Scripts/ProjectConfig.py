import os

PROJECT_CONFIG_FILE_NAME = '.projectconfig'

project_name = 'Unnamed'
project_name_shorthand = 'UN'
project_version_major = '0'
project_version_minor = '0'
project_version_patch = '0'
project_version_suffix = 'dev'
project_author = 'Unknown'

def LoadConfig():
	if not os.path.exists(PROJECT_CONFIG_FILE_NAME):
		print(f'Error: {PROJECT_CONFIG_FILE_NAME} not found.')
		exit(1)

	with open(PROJECT_CONFIG_FILE_NAME, 'r') as file:
		for line in file:
			key_value = line.split('=')
			if len(key_value) == 2:
				key = key_value[0].strip()
				value = key_value[1].strip()

				if key == 'NAME':
					global project_name
					project_name = value
				elif key == 'NAME_SHORTHAND':
					global project_name_shorthand
					project_name_shorthand = value
				elif key == 'VERSION_MAJOR':
					global project_version_major
					project_version_major = value
				elif key == 'VERSION_MINOR':
					global project_version_minor
					project_version_minor = value
				elif key == 'VERSION_PATCH':
					global project_version_patch
					project_version_patch = value
				elif key == 'VERSION_SUFFIX':
					global project_version_suffix
					project_version_suffix = value
				elif key == 'AUTHOR':
					global project_author
					project_author = value
			else:
				print(f'Error: Invalid key-value pair in {PROJECT_CONFIG_FILE_NAME}')
				exit(1)

def GetName():
	return project_name

def GetNameShorthand():
	return project_name_shorthand

def GetVersionMajor():
	return project_version_major

def GetVersionMinor():
	return project_version_minor

def GetVersionPatch():
	return project_version_patch

def GetVersion():
	return f'{GetVersionMajor()}.{GetVersionMinor()}.{GetVersionPatch()}'

def GetVersionSuffix():
	return project_version_suffix

def GetVersionString():
	return f'v{GetVersion()}-{GetVersionSuffix()}'

def GetAuthor():
	return project_author

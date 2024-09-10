import os

PROJECT_CONFIG_FILE_NAME = '.projectconfig'

project_name = 'Unnamed'
project_version = '0.0.0'
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
				elif key == 'VERSION':
					global project_version
					project_version = value
				elif key == 'VERSION_SUFFIX':
					global project_version_suffix
					project_version_suffix = value
				elif key == 'AUTHOR':
					global project_author
					project_author = value
			else:
				print(f'Error: Invalid key-value pair in {PROJECT_CONFIG_FILE_NAME}')
				exit(1)

def GetProjectName():
	return project_name

def GetProjectVersion():
	return project_version

def GetProjectVersionSuffix():
	return project_version_suffix

def GetProjectVersionString():
	return f'{GetProjectVersion()}-{GetProjectVersionSuffix()}'

def GetProjectAuthor():
	return project_author

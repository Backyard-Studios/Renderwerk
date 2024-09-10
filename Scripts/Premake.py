import Utils
import os
import subprocess
import ProjectConfig

PREMAKE_REQUIRED_VERSION = '5.0.0-beta2'
PREMAKE_DOWNLOAD_URL = f'https://github.com/premake/premake-core/releases/download/v{PREMAKE_REQUIRED_VERSION}/premake-{PREMAKE_REQUIRED_VERSION}-{Utils.GetPlatform().lower()}.zip'
PREMAKE_BINARY_FOLDER_PATH = 'Binaries/Tools/premake5'
PREMAKE_BINARY_FILE_PATH = f'{PREMAKE_BINARY_FOLDER_PATH}/premake5{Utils.GetExecutableExtension()}'
PREMAKE_CONFIG_FOLDER_PATH = 'Scripts/Config'

def IsInstalled():
	return Utils.DoesFileExist(PREMAKE_BINARY_FILE_PATH)

def Install():
	zip_file_path = f'{PREMAKE_BINARY_FOLDER_PATH}/premake.zip'

	Utils.CreateDirectoryIfNotExists(PREMAKE_BINARY_FOLDER_PATH)
	Utils.DownloadFile(PREMAKE_DOWNLOAD_URL, zip_file_path)
	Utils.UnzipFile(zip_file_path, PREMAKE_BINARY_FOLDER_PATH)
	os.remove(zip_file_path)

def ExecutePremakeCommand(*args):
	subprocess.call([PREMAKE_BINARY_FILE_PATH] + list(args))

def PreprocessProperties():
	properties_file_path = f'{PREMAKE_CONFIG_FOLDER_PATH}/properties.lua'
	properties_file_content = ''

	with open(properties_file_path, 'r') as file:
		properties_file_content = file.read()

	properties_file_content = properties_file_content.replace('$(PROJECT_NAME)', ProjectConfig.GetName())
	properties_file_content = properties_file_content.replace('$(PROJECT_NAME_SHORTHAND)', ProjectConfig.GetNameShorthand())
	properties_file_content = properties_file_content.replace('$(PROJECT_VERSION)', ProjectConfig.GetVersionString())
	properties_file_content = properties_file_content.replace('$(PROJECT_VERSION_SUFFIX)', ProjectConfig.GetVersionSuffix())
	properties_file_content = properties_file_content.replace('$(PROJECT_VERSION_STRING)', ProjectConfig.GetVersionString())
	properties_file_content = properties_file_content.replace('$(PROJECT_AUTHOR)', ProjectConfig.GetAuthor())

	properties_file_content = properties_file_content.replace('$(ROOT_PATH)', os.path.abspath('.').replace('\\', '/'))

	new_properties_file_path = f'{PREMAKE_CONFIG_FOLDER_PATH}/properties_preprocessed.lua'
	with open(new_properties_file_path, 'w') as file:
		file.write(properties_file_content)

import Utils
import os
import subprocess

PREMAKE_REQUIRED_VERSION = '5.0.0-beta2'
PREMAKE_DOWNLOAD_URL = f'https://github.com/premake/premake-core/releases/download/v{PREMAKE_REQUIRED_VERSION}/premake-{PREMAKE_REQUIRED_VERSION}-{Utils.GetPlatform().lower()}.zip'
PREMAKE_BINARY_FOLDER_PATH = 'Binaries/Tools/premake5'
PREMAKE_BINARY_FILE_PATH = f'{PREMAKE_BINARY_FOLDER_PATH}/premake5{Utils.GetExecutableExtension()}'

def IsPremakeInstalled():
	return Utils.DoesFileExist(PREMAKE_BINARY_FILE_PATH)

def InstallPremake():
	zip_file_path = f'{PREMAKE_BINARY_FOLDER_PATH}/premake.zip'

	Utils.CreateDirectoryIfNotExists(PREMAKE_BINARY_FOLDER_PATH)
	Utils.DownloadFile(PREMAKE_DOWNLOAD_URL, zip_file_path)
	Utils.UnzipFile(zip_file_path, PREMAKE_BINARY_FOLDER_PATH)
	os.remove(zip_file_path)

def ExecutePremakeCommand(*args):
	subprocess.call([PREMAKE_BINARY_FILE_PATH] + list(args))

import platform
import urllib.request
import os
import zipfile
import shutil

def IsWindows():
	return platform.system() == 'Windows'

def IsLinux():
	return platform.system() == 'Linux'

def GetPlatform():
	return platform.system()

def GetArchitecture():
	return platform.architecture()[0]

def GetExecutableExtension():
	if IsWindows():
		return '.exe'
	return ''

def DoesFileExist(file_path):
	return os.path.exists(file_path)

def DoesDirectoryExist(directory_path):
	return os.path.exists(directory_path)

def CreateDirectoryIfNotExists(directory_path):
	if not os.path.exists(directory_path):
		os.makedirs(directory_path)

def RemoveDirectory(directory_path):
	if os.path.exists(directory_path):
		shutil.rmtree(directory_path)

def RemoveFile(file_path):
	if os.path.exists(file_path):
		os.remove(file_path)

def RemoveAllFilesInDirectory(directory_path, file_extension):
	if os.path.exists(directory_path):
		for (dirname, dirs, files) in os.walk(directory_path):
			for file in files:
					if file.endswith(file_extension):
							source_file = os.path.join(dirname, file)
							os.remove(source_file)

def MoveFile(source_path, destination_path):
	if os.path.exists(source_path):
		shutil.move(source_path, destination_path)

def MoveDirectory(source_path, destination_path):
	if os.path.exists(source_path):
		shutil.move(source_path, destination_path)

def DownloadFile(url, output_path):
	if not os.path.exists(output_path):
		urllib.request.urlretrieve(url, output_path)
	else:
		print(f'{output_path} already exists, skipping download.')

def UnzipFile(file_path, output_folder):
	with zipfile.ZipFile(file_path, 'r') as zip_ref:
		zip_ref.extractall(output_folder)

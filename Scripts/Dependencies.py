import Utils

def Install():
	if IsAgilitySDKInstalled() == False:
		InstallAgilitySDK()
	if IsDXCInstalled() == False:
		InstallDXC()

# []===========================================================================[]

AGILITY_SDK_PATH = 'Engine/ThirdParty/AgilitySDK'
AGILITY_SDK_DOWNLOAD_URL = 'https://www.nuget.org/api/v2/package/Microsoft.Direct3D.D3D12/1.614.1'
AGILITY_SDK_DOWNLOAD_FILE_NAME = 'microsoft.direct3d.d3d12.nupkg'
AGILITY_SDK_DOWNLOAD_PATH = f'{AGILITY_SDK_PATH}/download'
AGILITY_SDK_DOWNLOAD_FILE_PATH = f'{AGILITY_SDK_DOWNLOAD_PATH}/{AGILITY_SDK_DOWNLOAD_FILE_NAME}'

def IsAgilitySDKInstalled():
	return Utils.DoesDirectoryExist(AGILITY_SDK_PATH) and Utils.DoesDirectoryExist(f'{AGILITY_SDK_PATH}/bin') and Utils.DoesDirectoryExist(f'{AGILITY_SDK_PATH}/include') and Utils.DoesDirectoryExist(f'{AGILITY_SDK_PATH}/src')

def InstallAgilitySDK():
	if Utils.DoesDirectoryExist(AGILITY_SDK_PATH):
		Utils.RemoveDirectory(AGILITY_SDK_PATH)
	Utils.CreateDirectoryIfNotExists(AGILITY_SDK_DOWNLOAD_PATH)
	print('Downloading Agility SDK...')
	Utils.DownloadFile(AGILITY_SDK_DOWNLOAD_URL, AGILITY_SDK_DOWNLOAD_FILE_PATH)
	print('Extracting Agility SDK...')
	Utils.UnzipFile(AGILITY_SDK_DOWNLOAD_FILE_PATH, AGILITY_SDK_DOWNLOAD_PATH)
	Utils.RemoveFile(AGILITY_SDK_DOWNLOAD_FILE_PATH)

	binary_path = f'{AGILITY_SDK_DOWNLOAD_PATH}/build/native/bin'
	Utils.MoveDirectory(binary_path, AGILITY_SDK_PATH)

	include_path = f'{AGILITY_SDK_DOWNLOAD_PATH}/build/native/include'
	Utils.MoveDirectory(include_path, AGILITY_SDK_PATH)

	src_path = f'{AGILITY_SDK_DOWNLOAD_PATH}/build/native/src'
	Utils.MoveDirectory(src_path, AGILITY_SDK_PATH)

	Utils.RemoveDirectory(AGILITY_SDK_DOWNLOAD_PATH)

	print('Agility SDK installed')

# []===========================================================================[]

DXC_PATH = 'Engine/ThirdParty/DirectXCompiler'
DXC_DOWNLOAD_URL = 'https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.8.2407/dxc_2024_07_31.zip'
DXC_DOWNLOAD_FILE_NAME = 'dxc.zip'
DXC_DOWNLOAD_PATH = f'{DXC_PATH}/download'
DXC_DOWNLOAD_FILE_PATH = f'{DXC_DOWNLOAD_PATH}/{DXC_DOWNLOAD_FILE_NAME}'


def IsDXCInstalled():
	return Utils.DoesDirectoryExist(DXC_PATH) and Utils.DoesDirectoryExist(f'{DXC_PATH}/bin') and Utils.DoesDirectoryExist(f'{DXC_PATH}/inc') and Utils.DoesDirectoryExist(f'{DXC_PATH}/lib')

def InstallDXC():
	if Utils.DoesDirectoryExist(DXC_PATH):
		Utils.RemoveDirectory(DXC_PATH)
	Utils.CreateDirectoryIfNotExists(DXC_DOWNLOAD_PATH)
	print('Downloading DXC...')
	Utils.DownloadFile(DXC_DOWNLOAD_URL, DXC_DOWNLOAD_FILE_PATH)
	print('Extracting DXC...')
	Utils.UnzipFile(DXC_DOWNLOAD_FILE_PATH, DXC_DOWNLOAD_PATH)
	Utils.RemoveFile(DXC_DOWNLOAD_FILE_PATH)

	binary_path = f'{DXC_DOWNLOAD_PATH}/bin'
	Utils.MoveDirectory(binary_path, DXC_PATH)

	include_path = f'{DXC_DOWNLOAD_PATH}/inc'
	Utils.MoveDirectory(include_path, DXC_PATH)

	lib_path = f'{DXC_DOWNLOAD_PATH}/lib'
	Utils.MoveDirectory(lib_path, DXC_PATH)

	Utils.RemoveDirectory(DXC_DOWNLOAD_PATH)

	print('DXC installed')

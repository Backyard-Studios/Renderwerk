import os
import Utils
import subprocess

VULKAN_SDK_PATH = os.environ.get('VULKAN_SDK')
VULKAN_REQUIRED_VERSION = '1.3.'
VULKAN_INSTALL_VERSION = '1.3.290.0'
VULKAN_WINDOWS_DOWNLOAD_URL = f'https://sdk.lunarg.com/sdk/download/{VULKAN_INSTALL_VERSION}/windows/VulkanSDK-{VULKAN_INSTALL_VERSION}-Installer.exe'
VULKAN_INSTALLER_PATH = 'Binaries/Tools/VulkanSDK'

def IsInstalled():
	if VULKAN_SDK_PATH is None:
		return False
	elif not VULKAN_REQUIRED_VERSION in VULKAN_SDK_PATH:
		return False
	if Utils.DoesFileExist(f'{VULKAN_INSTALLER_PATH}/VulkanSDK-{VULKAN_INSTALL_VERSION}-Installer.exe'):
		os.remove(f'{VULKAN_INSTALLER_PATH}/VulkanSDK-{VULKAN_INSTALL_VERSION}-Installer.exe')
	return True

def Install():
	print('Vulkan SDK not found, installing...')
	Utils.CreateDirectoryIfNotExists(VULKAN_INSTALLER_PATH)
	print(f'Downloading Vulkan SDK {VULKAN_INSTALL_VERSION}...')
	Utils.DownloadFile(VULKAN_WINDOWS_DOWNLOAD_URL, f'{VULKAN_INSTALLER_PATH}/VulkanSDK-{VULKAN_INSTALL_VERSION}-Installer.exe')
	print('Running Vulkan SDK installer...')
	subprocess.call([f'{VULKAN_INSTALLER_PATH}/VulkanSDK-{VULKAN_INSTALL_VERSION}-Installer.exe'])
	print('Please rerun the setup script after the Vulkan SDK installation has completed.')

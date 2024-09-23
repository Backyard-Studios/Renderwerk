import os
import subprocess

import Utils
import Premake
import Vulkan
import ProjectConfig

if Utils.GetArchitecture() != '64bit':
	print('Error: Only 64-bit architectures are supported.')
	exit(1)

# Pull the LFS files and update the submodules
subprocess.call(["git", "lfs", "pull"])
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

if Vulkan.IsInstalled() == False:
	Vulkan.Install()
	exit(0)

if Premake.IsInstalled() == False:
	Premake.Install()

ProjectConfig.LoadConfig()
Premake.PreprocessProperties()

# Generate the project files
Premake.ExecutePremakeCommand('vs2022', '--file=Scripts/Config/premake5.lua')

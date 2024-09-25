import os
import subprocess

import Utils
import Premake
import Dependencies
import ProjectConfig

if Utils.GetArchitecture() != '64bit':
	print('Error: Only 64-bit architectures are supported.')
	exit(1)

# Pull the LFS files and update the submodules
subprocess.call(["git", "lfs", "pull"])
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

if Premake.IsInstalled() == False:
	Premake.Install()

if Dependencies.IsAgilitySDKInstalled() == False:
	Dependencies.InstallAgilitySDK()

ProjectConfig.LoadConfig()
Premake.PreprocessProperties()

# Generate the project files
Premake.ExecutePremakeCommand('vs2022', '--file=Scripts/Config/premake5.lua')

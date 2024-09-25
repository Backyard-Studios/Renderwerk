import Utils

def Execute():
  print('Cleaning...')
  Utils.RemoveDirectory('Binaries')
  Utils.RemoveDirectory('Intermediate')
  Utils.RemoveDirectory('Intermediate')
  Utils.RemoveAllFilesInDirectory('Engine', '.dmp')
  Utils.RemoveAllFilesInDirectory('Engine', '.vcxproj')
  Utils.RemoveAllFilesInDirectory('Engine', '.vcxproj.filters')
  Utils.RemoveFile('Renderwerk.sln')
  Utils.RemoveFile('Renderwerk.sln.DotSettings.user')
  print('Done')

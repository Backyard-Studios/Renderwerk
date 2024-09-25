import Utils

def Execute():
  print('Cleaning...')
  Utils.RemoveDirectory('Binaries')
  Utils.RemoveDirectory('Intermediate')
  Utils.RemoveDirectory('Engine/ThirdParty/AgilitySDK')
  print('Done')

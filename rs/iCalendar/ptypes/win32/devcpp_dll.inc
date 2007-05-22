#
# Pre-build and post-build steps for Dev-C++,
# used from PTypes_DLL and PTypes_Test projects
#

all-before:
	@cmd /C IF NOT EXIST DLL_Release mkdir DLL_Release

all-after:
	@cmd /C IF NOT EXIST ..\so mkdir ..\so
	@cmd /C IF EXIST DLL_Release\ptypes21g.dll copy DLL_Release\ptypes21g.dll ..\so > NUL
	@cmd /C IF EXIST ..\so\ptypes21g.dll strip ..\so\ptypes21g.dll

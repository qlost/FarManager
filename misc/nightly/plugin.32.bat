call %~dp0base_32.bat

nmake /f makefile_vc FAR_WORKDIR=..\..\outfinalnew32 BUILD=1 LUA=luasdk\20241113\64\lua.exe

@echo off
setlocal enableextensions enabledelayedexpansion

set /a t=( ( %RANDOM% * %RANDOM% ) %% 900000 + 100000 )
set t=%SystemRoot%\csurun-install-%t%
( set -p "x=" <nul >%t% ) 2>nul || (
	surun "%~dpnx0" && goto test
	echo\installation canceled>&2
	exit /b 1
)
del %t%
pushd "%~dp0\.."
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
	cd x64
) else if "%PROCESSOR_ARCHITECTURE%,%PROCESSOR_ARCHITEW6432%"=="x86,AMD64" (
	cd x64
)
cd Release
prompt %PROMPT%$_
echo on
copy cSuRun.exe "%SystemRoot%"
copy cSuRunH.bin "%SystemRoot%"
@echo off
prompt %PROMPT:~0,-2%
popd
echo\
pause
exit /b

:test
echo\
echo\wait until installation has finished
pause
csurun cmd /c "mklink /d "%t%" . && rmdir "%t%""
pause

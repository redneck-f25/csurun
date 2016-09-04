@echo off
setlocal enableextensions enabledelayedexpansion

where "surun.exe" >nul 2>nul || goto no_surun
net session >nul 2>nul || goto sudo

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

pause
exit /b


:sudo
surun "%~0"
exit /b

:no_surun

echo\SuRun not found.
pause
exit /b

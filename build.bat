@echo off

setlocal

cd %~dp0

if not exist build mkdir build
cd build

if "%Platform%" neq "x64" (
	echo ERROR: Platform is not "x64" - please run this from the MSVC x64 native tools command prompt.
	goto end
)

set "common_compile_options="
set "common_link_options= /incremental:no /opt:ref /subsystem:console"

if "%1"=="debug" (
	set "compile_options=%common_compile_options% /Od /Zo /Z7 /RTC1"
	set "link_options=%common_link_options%"
) else if "%1"=="release" (
	set "compile_options=%common_compile_options% /O2"
	set "link_options=%common_link_options%"
) else goto invalid_arguments

if "%2" neq "" goto invalid_arguments

cl %compile_options% ..\src\r_main.c /link %link_options% /pdb:rin.pdb /out:rin.exe

goto end

:invalid_arguments
echo Invalid arguments^. Usage: build ^<debug^|release^>
goto end

:end
endlocal

@echo off
rem -- Run Vim --

set VIM=C:\Program Files\Vim

if exist "%VIM%\vim63\gvim.exe" goto havevim
echo "%VIM%\vim63\gvim.exe" not found
goto eof

:havevim
rem collect the arguments in VIMARGS for Win95
set VIMARGS=
set VIMNOFORK=
:loopstart
if .%1==. goto loopend
if NOT .%1==.-f goto noforkarg
set VIMNOFORK=1
:noforkarg
set VIMARGS=%VIMARGS% %1
shift
goto loopstart

:loopend
if .%OS%==.Windows_NT goto ntaction

if .%VIMNOFORK%==.1 goto nofork
start "%VIM%\vim63\gvim.exe"  %VIMARGS%
goto eof

:nofork
start /w "%VIM%\vim63\gvim.exe"  %VIMARGS%
goto eof

:ntaction
rem for WinNT we can use %*
if .%VIMNOFORK%==.1 goto noforknt
start "dummy" /b "%VIM%\vim63\gvim.exe"  %*
goto eof

:noforknt
start "dummy" /b /wait "%VIM%\vim63\gvim.exe"  %*

:eof
set VIMARGS=
set VIMNOFORK=

@echo off
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0"
title Whack-a-Mole - build and run

rem ---------------------------------------------------------------
rem  Builds and plays the game. Needs Qt with the MinGW kit
rem  (installed together with Qt Creator).
rem
rem     run_windows.bat                 build and play
rem     run_windows.bat --standalone    also create the folder
rem                                     WhackAMole-standalone that runs
rem                                     on a PC without Qt
rem     run_windows.bat --no-run        only build
rem ---------------------------------------------------------------

set "STANDALONE=0"
set "RUN=1"
:parse
if "%~1"=="" goto parsed
if /i "%~1"=="--standalone" set "STANDALONE=1"
if /i "%~1"=="--no-run" set "RUN=0"
shift
goto parse
:parsed

rem ---- 1. find qmake -------------------------------------------------
set "QMAKE="
for /f "delims=" %%Q in ('where qmake 2^>nul') do if not defined QMAKE set "QMAKE=%%Q"
if defined QMAKE goto havemake_qmake

for %%R in ("C:\Qt" "D:\Qt" "%USERPROFILE%\Qt") do (
    if exist "%%~R\" (
        for /f "delims=" %%V in ('dir /b /ad "%%~R" 2^>nul') do (
            for /f "delims=" %%K in ('dir /b /ad "%%~R\%%V\mingw*" 2^>nul') do (
                if exist "%%~R\%%V\%%K\bin\qmake.exe" set "QMAKE=%%~R\%%V\%%K\bin\qmake.exe"
            )
        )
    )
)
if not defined QMAKE goto noqt

:havemake_qmake
for %%I in ("%QMAKE%\..") do set "QTBIN=%%~fI"
for %%I in ("%QMAKE%\..\..") do set "KITDIR=%%~fI"
for %%I in ("%QMAKE%\..\..\..\..") do set "QTROOT=%%~fI"
for %%I in ("%KITDIR%") do set "KITNAME=%%~nxI"
set "PATH=%QTBIN%;%PATH%"

if /i not "!KITNAME:~0,5!"=="mingw" goto notmingwkit

rem ---- 2. find the MinGW compiler that belongs to this Qt kit --------
set "MINGWBIN="
where mingw32-make >nul 2>nul
if not errorlevel 1 goto mingwfound

rem   Qt 5 kits are named like mingw81_64 and use Tools\mingw810_64.
rem   Qt 6 kits are named mingw_64; for those we take the newest compiler.
set "WANT="
set "KITNUM=!KITNAME:mingw=!"
set "KITNUM=!KITNUM:_64=!"
if not "!KITNUM!"=="" set "WANT=mingw!KITNUM!0_64"
if defined WANT if exist "%QTROOT%\Tools\%WANT%\bin\mingw32-make.exe" set "MINGWBIN=%QTROOT%\Tools\%WANT%\bin"
if defined MINGWBIN goto mingwfound

set "BEST=0000"
for /f "delims=" %%T in ('dir /b /ad "%QTROOT%\Tools\mingw*" 2^>nul') do call :considertools "%%T"

:mingwfound
if defined MINGWBIN set "PATH=%MINGWBIN%;%PATH%"

where mingw32-make >nul 2>nul
if errorlevel 1 goto nomingw
where g++ >nul 2>nul
if errorlevel 1 goto nomingw

echo Using Qt:     %QMAKE%
echo Compiler dir: %MINGWBIN%

rem ---- 3. build (in the "build" folder, so your files stay clean) ----
if not exist build mkdir build
pushd build
"%QMAKE%" ..\WhackAMole.pro -spec win32-g++ "CONFIG+=release"
if errorlevel 1 goto fail
mingw32-make -j4
if errorlevel 1 goto fail
popd

set "EXE="
if exist "build\release\WhackAMole.exe" set "EXE=build\release\WhackAMole.exe"
if not defined EXE if exist "build\WhackAMole.exe" set "EXE=build\WhackAMole.exe"
if not defined EXE goto fail

rem ---- 4. optional: standalone folder --------------------------------
if not "%STANDALONE%"=="1" goto skipstandalone
if exist WhackAMole-standalone rmdir /s /q WhackAMole-standalone
mkdir WhackAMole-standalone
copy /y "%EXE%" WhackAMole-standalone\ >nul
windeployqt --no-translations WhackAMole-standalone\WhackAMole.exe
if errorlevel 1 goto fail
echo.
echo Created the folder WhackAMole-standalone - it runs without Qt.
echo To share it, right-click the folder and choose Send to, Compressed folder.
:skipstandalone

rem ---- 5. run ----------------------------------------------------------
if not "%RUN%"=="1" exit /b 0
echo Starting the game...
"%EXE%"
exit /b 0

:noqt
echo Could not find Qt on this computer.
echo.
echo Install it once, then run this file again:
echo   1. Download the Qt Online Installer from https://www.qt.io/download-open-source
echo   2. Choose a custom installation
echo   3. Tick MinGW 64-bit for the newest Qt version, and keep Qt Creator
echo      and the MinGW compiler under Developer and Designer Tools ticked
echo.
echo Or use WhackAMole_Launcher.bat and choose option 5 to install it.
pause
exit /b 1

:notmingwkit
echo The Qt that was found (%KITNAME%) is not a MinGW kit, and this script
echo only builds with MinGW. Open WhackAMole.pro in Qt Creator instead
echo and press Run, or install the MinGW 64-bit kit.
pause
exit /b 1

:nomingw
echo Qt was found, but the MinGW compiler was not.
echo Open the Qt Maintenance Tool, choose Add or remove components, and tick
echo MinGW under Developer and Designer Tools. Or open WhackAMole.pro in Qt Creator.
pause
exit /b 1

:fail
echo.
echo Something went wrong - read the messages above.
echo Tip: you can always open WhackAMole.pro in Qt Creator and press Run instead.
pause
exit /b 1

:considertools
set "T=%~1"
set "N=!T:mingw=!"
set "N=!N:_64=!"
set "N=0000!N!"
set "N=!N:~-4!"
if not exist "%QTROOT%\Tools\%~1\bin\mingw32-make.exe" exit /b 0
if "!N!" gtr "!BEST!" (
    set "BEST=!N!"
    set "MINGWBIN=%QTROOT%\Tools\%~1\bin"
)
exit /b 0

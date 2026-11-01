@echo off
setlocal EnableExtensions
cd /d "%~dp0"
title Whack-a-Mole launcher

rem ---------------------------------------------------------------
rem  Double-click this file. It gives you a menu:
rem    1  play the game (builds it first)
rem    2  open the code in Qt Creator, to read and analyse it
rem    3  open the PDF guide
rem    4  open the README
rem    5  install Qt and Qt Creator (only needed if they are missing)
rem ---------------------------------------------------------------

:menu
cls
echo ==============================================================
echo    Whack-a-Mole  -  launcher
echo ==============================================================
echo.
echo    [1]  Play the game               (builds it, then runs it)
echo    [2]  Open the code in Qt Creator (read and analyse)
echo    [3]  Open the PDF guide
echo    [4]  Open the README
echo    [5]  Install Qt and Qt Creator   (only if they are missing)
echo    [6]  Exit
echo.
choice /c 123456 /n /m "Choose 1-6: "
if errorlevel 6 exit /b 0
if errorlevel 5 goto install
if errorlevel 4 goto readme
if errorlevel 3 goto pdf
if errorlevel 2 goto openqc
if errorlevel 1 goto play
goto menu

rem ---------------------------------------------------------------
:play
if not exist "%~dp0run_windows.bat" goto missing
call "%~dp0run_windows.bat"
echo.
pause
goto menu

rem ---------------------------------------------------------------
:openqc
set "QTC="
for /f "delims=" %%Q in ('where qtcreator 2^>nul') do if not defined QTC set "QTC=%%Q"
if defined QTC goto foundqc
for %%R in ("C:\Qt\Tools\QtCreator\bin\qtcreator.exe" "D:\Qt\Tools\QtCreator\bin\qtcreator.exe" "%USERPROFILE%\Qt\Tools\QtCreator\bin\qtcreator.exe" "%ProgramFiles%\Qt Creator\bin\qtcreator.exe" "%LOCALAPPDATA%\Programs\Qt Creator\bin\qtcreator.exe") do if exist %%R if not defined QTC set "QTC=%%~R"
if not defined QTC goto noqc

:foundqc
echo.
echo Opening the project in Qt Creator...
start "" "%QTC%" "%~dp0WhackAMole.pro"
echo.
echo When Qt Creator asks to configure the project, tick the kit named
echo "Desktop Qt ... MinGW 64-bit" and press Configure Project.
echo Then open the files from the left panel: main.cpp, game_window.h,
echo game_window.cpp, clock_label.cpp.
echo.
pause
goto menu

:noqc
echo.
echo Qt Creator was not found automatically.
echo.
echo   - If it is installed: open it from the Start menu, then choose
echo     File, Open File or Project..., and select WhackAMole.pro in
echo     this folder.
echo   - If it is not installed: go back and choose option 5.
echo.
pause
goto menu

rem ---------------------------------------------------------------
:pdf
if not exist "%~dp0WhackAMole_Guide.pdf" goto missing
start "" "%~dp0WhackAMole_Guide.pdf"
goto menu

rem ---------------------------------------------------------------
:readme
if not exist "%~dp0README.md" goto missing
start "" notepad "%~dp0README.md"
goto menu

rem ---------------------------------------------------------------
:install
cls
echo ==============================================================
echo    Install Qt and Qt Creator
echo ==============================================================
echo.
echo This downloads the official Qt installer from qt.io and starts it.
echo The installer is a normal window: you click through it yourself
echo and sign in with a free Qt account. It cannot be done silently.
echo.
echo When it asks what to install:
echo   1. Choose "Custom installation".
echo   2. Under the newest Qt version, tick ONLY "MinGW 64-bit".
echo   3. Keep "Qt Creator" and the MinGW compiler under
echo      "Developer and Designer Tools" ticked.
echo   4. Leave everything else unticked (smaller download).
echo.
choice /c YN /m "Download and start the installer now"
if errorlevel 2 goto menu

set "INST=%TEMP%\qt-online-installer.exe"
if exist "%INST%" del "%INST%"
echo.
echo Downloading... please wait.
powershell -NoProfile -ExecutionPolicy Bypass -Command "$ProgressPreference='SilentlyContinue'; [Net.ServicePointManager]::SecurityProtocol=[Net.SecurityProtocolType]::Tls12; $urls=@('https://download.qt.io/official_releases/online_installers/qt-online-installer-windows-x64-online.exe','https://download.qt.io/official_releases/online_installers/qt-unified-windows-x64-online.exe'); foreach($u in $urls){ try{ Invoke-WebRequest -Uri $u -OutFile '%INST%' -UseBasicParsing -ErrorAction Stop; break } catch { Write-Host ('Could not download ' + $u) } }"
if not exist "%INST%" goto dlfail

echo.
echo Starting the installer. Follow the steps listed above.
echo After it finishes, run this launcher again and choose 1 or 2.
start "" "%INST%"
echo.
pause
goto menu

:dlfail
echo.
echo The download did not work. Opening the Qt download page instead.
echo Download "Qt Online Installer for Windows" there and follow the
echo same steps as above.
start "" "https://www.qt.io/download-open-source"
echo.
pause
goto menu

rem ---------------------------------------------------------------
:missing
echo.
echo A file this option needs is missing from the folder.
echo Make sure you unzipped the whole WhackAMole.zip and did not move files.
echo.
pause
goto menu

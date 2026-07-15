@echo off
REM BioHiveEvolution - 一键完整设置脚本
REM 此脚本将在UE5安装后自动完成所有项目设置

echo ========================================
echo BioHive Evolution - Complete Setup
echo ========================================
echo.

set UE5_PATH=C:\Program Files\Epic Games\UE_5.4
set PROJECT_DIR=%~dp0
set PROJECT_FILE=%PROJECT_DIR%BioHiveEvolution.uproject

REM 检查UE5
if not exist "%UE5_PATH%" (
    echo [ERROR] UE5.4 not found!
    echo.
    echo Please install UE5.4 from:
    echo https://www.unrealengine.com/zh-CN/download
    echo.
    pause
    exit /b 1
)

echo [1/7] Checking UE5 installation...
echo   Found: %UE5_PATH%
echo.

echo [2/7] Generating project files...
"%UE5_PATH%\Engine\Build\BatchFiles\GenerateProjectFiles.bat" "%PROJECT_FILE%"
if errorlevel 1 (
    echo [ERROR] Failed to generate project files
    pause
    exit /b 1
)
echo   Success!
echo.

echo [3/7] Building C++ code (Development Editor)...
echo   This may take 5-10 minutes...
"%UE5_PATH%\Engine\Build\BatchFiles\Build.bat" BioHiveEvolutionEditor Win64 Development "%PROJECT_FILE%" -waitmutex
if errorlevel 1 (
    echo [ERROR] Build failed
    echo   Check Visual Studio output for details
    pause
    exit /b 1
)
echo   Build completed!
echo.

echo [4/7] Launching UE5 Editor...
echo   The editor will open automatically
start "" "%UE5_PATH%\Engine\Binaries\Win64\UnrealEditor.exe" "%PROJECT_FILE%" -run=pythonscript -script="%PROJECT_DIR%Scripts\auto_setup_ue5.py"

echo.
echo [5/7] Waiting for editor to load...
timeout /t 30 /nobreak >nul

echo.
echo [6/7] IMPORTANT - Manual Steps Required:
echo.
echo   After the editor opens, please:
echo.
echo   1. Enable Quixel Bridge plugin:
echo      - Edit ^> Plugins ^> Search "Quixel" ^> Enable
echo      - Restart editor when prompted
echo.
echo   2. Import forest assets from Quixel Bridge:
echo      - Window ^> Quixel Bridge
echo      - Search "Forest" and download trees/rocks
echo.
echo   3. Create Blueprint classes:
echo      - See README.md section "Creating Blueprints"
echo      - Or run Scripts/auto_setup_ue5.py in editor Python console
echo.
echo   4. Set up test level:
echo      - Open Content/Maps/L_TestForest
echo      - Add landscape and resource nodes
echo.

echo [7/7] Setup script completed!
echo.
echo ========================================
echo Next Steps:
echo ========================================
echo.
echo 1. Follow the manual steps above
echo 2. Read README.md for detailed instructions
echo 3. Check PROJECT_COMPLETE_REPORT.md for status
echo.
echo For support, see:
echo   - README.md
echo   - PROJECT_COMPLETE_REPORT.md  
echo   - DIR.txt
echo.
pause

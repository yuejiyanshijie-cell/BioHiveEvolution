@echo off
REM BioHiveEvolution - 项目启动脚本
REM 用于快速编译和启动项目

echo ========================================
echo BioHive Evolution - Project Launcher
echo ========================================
echo.

REM 检查UE5安装路径
set UE5_PATH=C:\Program Files\Epic Games\UE_5.4
if not exist "%UE5_PATH%" (
    echo [ERROR] UE5 not found at %UE5_PATH%
    echo Please install UE5.4 from Epic Games Launcher
    pause
    exit /b 1
)

echo [INFO] Found UE5 at: %UE5_PATH%
echo.

REM 获取项目路径
set PROJECT_DIR=%~dp0
set PROJECT_FILE=%PROJECT_DIR%BioHiveEvolution.uproject

echo [INFO] Project directory: %PROJECT_DIR%
echo.

REM 菜单选择
echo Select action:
echo 1. Generate project files (Visual Studio)
echo 2. Build project (Development Editor)
echo 3. Launch editor
echo 4. Build and launch
echo 5. Package game (Windows)
echo 6. Clean intermediate files
echo.
set /p ACTION="Enter choice (1-6): "

if "%ACTION%"=="1" goto generate
if "%ACTION%"=="2" goto build
if "%ACTION%"=="3" goto launch
if "%ACTION%"=="4" goto build_and_launch
if "%ACTION%"=="5" goto package
if "%ACTION%"=="6" goto clean
goto end

:generate
echo.
echo [INFO] Generating Visual Studio project files...
"%UE5_PATH%\Engine\Build\BatchFiles\GenerateProjectFiles.bat" "%PROJECT_FILE%"
if errorlevel 1 (
    echo [ERROR] Failed to generate project files
    pause
    exit /b 1
)
echo [SUCCESS] Project files generated!
pause
goto end

:build
echo.
echo [INFO] Building project (Development Editor)...
"%UE5_PATH%\Engine\Build\BatchFiles\Build.bat" BioHiveEvolutionEditor Win64 Development "%PROJECT_FILE%"
if errorlevel 1 (
    echo [ERROR] Build failed
    pause
    exit /b 1
)
echo [SUCCESS] Build completed!
pause
goto end

:launch
echo.
echo [INFO] Launching UE5 Editor...
start "" "%UE5_PATH%\Engine\Binaries\Win64\UnrealEditor.exe" "%PROJECT_FILE%"
goto end

:build_and_launch
echo.
echo [INFO] Building project...
call :build
if errorlevel 1 goto end

echo.
echo [INFO] Launching editor...
goto launch

:package
echo.
echo [INFO] Packaging game for Windows...
"%UE5_PATH%\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun ^
    -project="%PROJECT_FILE%" ^
    -noP4 ^
    -platform=Win64 ^
    -clientconfig=Shipping ^
    -cook ^
    -allmaps ^
    -stage ^
    -pak ^
    -archive ^
    -archivedirectory="%PROJECT_DIR%Builds"

if errorlevel 1 (
    echo [ERROR] Packaging failed
    pause
    exit /b 1
)
echo [SUCCESS] Game packaged to: %PROJECT_DIR%Builds
pause
goto end

:clean
echo.
echo [INFO] Cleaning intermediate files...
if exist "%PROJECT_DIR%Intermediate" rmdir /s /q "%PROJECT_DIR%Intermediate"
if exist "%PROJECT_DIR%DerivedDataCache" rmdir /s /q "%PROJECT_DIR%DerivedDataCache"
if exist "%PROJECT_DIR%.vs" rmdir /s /q "%PROJECT_DIR%.vs"
if exist "%PROJECT_DIR%Binaries" rmdir /s /q "%PROJECT_DIR%Binaries"
echo [SUCCESS] Cleaned!
pause
goto end

:end
echo.
echo Done.
exit /b 0

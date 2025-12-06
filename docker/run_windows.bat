@echo off
setlocal

REM Get the directory of this script
set "SCRIPT_DIR=%~dp0"
REM Remove trailing backslash
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"

REM Get project root (parent directory)
for %%I in ("%SCRIPT_DIR%\..") do set "PROJECT_ROOT=%%~fI"

echo Building Docker image...
docker build -t ft_minecraft -f "%SCRIPT_DIR%\Dockerfile" "%PROJECT_ROOT%"

echo.
echo =================================================================
echo NOTE: You must have an X Server (like VcXsrv) running on Windows.
echo Configuration: "Multiple windows", "Start no client", 
echo and CRITICAL: Check "Disable access control".
echo =================================================================
echo.

echo Running ft_minecraft...
docker run --gpus all -it --rm ^
    -e DISPLAY=host.docker.internal:0.0 ^
    -v "%PROJECT_ROOT%:/app" ^
    ft_minecraft

endlocal
pause
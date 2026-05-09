@echo off
REM Batch file for building for_test.c on Windows

setlocal enabledelayedexpansion

REM Compiler and flags
set CC=C:\TDM-GCC\bin\gcc.exe
set CFLAGS=-Wall -Wextra -g3
set OUTPUT_DIR=output
set TARGET=%OUTPUT_DIR%\for_test.exe

REM Create output directory if it doesn't exist
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

REM Compile for_test.c and sha256.c together
echo Building %TARGET%...
%CC% %CFLAGS% for_test.c core\sha256.c -o %TARGET%

if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo Build complete: %TARGET%

@echo off
title Crossing Game
cd /d "%~dp0bin\x64\Release"
if not exist "CrossingGame.exe" (
    echo [ERROR] Chua tim thay CrossingGame.exe trong thu muc bin\x64\Release!
    echo Dang tien hanh build Release x64 bang MSBuild...
    cd /d "%~dp0"
    "%ProgramFiles%\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\amd64\MSBuild.exe" CrossingGame.sln -p:Configuration=Release -p:Platform=x64
    cd /d "%~dp0bin\x64\Release"
)
if exist "CrossingGame.exe" (
    start "" "CrossingGame.exe"
) else (
    echo [ERROR] Khong the khoi dong game. Vui long mo CrossingGame.sln trong Visual Studio de Build.
    pause
)

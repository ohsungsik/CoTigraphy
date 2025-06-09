@echo off
@chcp 65001 > nul
@setlocal EnableDelayedExpansion

@REM 현재 배치 파일이 있는 디렉터리
set "SCRIPT_DIR=%~dp0"

@REM Visual Studio 개발 환경이 이미 설정되어 있는지 확인
if not defined VCINSTALLDIR (
    @REM Visual Studio 설치 경로 찾기
    for /f "usebackq tokens=1* delims=: " %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.Component.MSBuild`) do (
        if /i "%%i"=="installationPath" (
            set VS_PATH=%%j
        )
    )

    if not defined VS_PATH (
        echo Error: Visual Studio installation not found.
        echo Please ensure Visual Studio with C++ workload is installed.
        goto cleanup
    )

    @REM Visual Studio 환경 변수 설정
    set "VSDEVCMD_PATH=!VS_PATH!\Common7\Tools\VsDevCmd.bat"
    if not exist "!VSDEVCMD_PATH!" (
        echo Error: VsDevCmd.bat not found at expected location: !VSDEVCMD_PATH!
        goto cleanup
    )

    @REM Visual Studio 개발 환경 초기화
    call "!VSDEVCMD_PATH!" > nul
    if errorlevel 1 (
        echo Error: Failed to initialize VS development environment
        goto cleanup
    )
)

@REM 외부 라이브러리 목록
set "THIRD_PARTIES=googletest-1.17.0 curl-8.14.1"

for %%l in (%THIRD_PARTIES%) do (
    set "BUILD_SCRIPT=%SCRIPT_DIR%%%l\build.bat"
    cmd /C "set "PAUSE_DISABLED=1" && "!BUILD_SCRIPT!""
    if errorlevel 1 (
        goto cleanup
    )
)

if "!PAUSE_DISABLED!" neq "1" (
    pause
)
endlocal
exit /b 0

:cleanup
if "!PAUSE_DISABLED!" neq "1" (
    pause
)
endlocal
exit /b 1
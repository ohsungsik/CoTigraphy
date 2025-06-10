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
        goto cleanup_failed
    )

    @REM Visual Studio 환경 변수 설정
    set "VSDEVCMD_PATH=!VS_PATH!\Common7\Tools\VsDevCmd.bat"
    if not exist "!VSDEVCMD_PATH!" (
        echo Error: VsDevCmd.bat not found at expected location: !VSDEVCMD_PATH!
        goto cleanup_failed
    )

    @REM Visual Studio 개발 환경 초기화
    call "!VSDEVCMD_PATH!" > nul
    if errorlevel 1 (
        echo Error: Failed to initialize VS development environment
        goto cleanup_failed
    )
)

@REM 빌드 대상 구성
set "CONFIGS=Debug Release"
set "PLATFORMS=x64 x86 ARM64"
set "HAS_FAILED=0"
set "BUILD_COUNT=0"

@REM 결과 저장을 위한 배열 사용
set "RESULT_PREFIX=BUILD_RESULT_"

@REM Color codes
set "COLOR_GREEN=[32m"
set "COLOR_RED=[31m"
set "COLOR_RESET=[0m"

@REM 모든 .sln 파일에 대해서 빌드
for %%S in ("!SCRIPT_DIR!\*.sln") do (
    echo ===================================================
    echo Building solution: %%S
    echo ===================================================
    
    for %%C in (%CONFIGS%) do (
        for %%P in (%PLATFORMS%) do (
            echo [INFO] Building %%C^|%%P for solution %%S ...
            
            @REM MSBuild 명령 실행
            call MSBuild "%%S" /p:Configuration=%%C /p:Platform=%%P /t:Rebuild /m
            
            if errorlevel 1 (
                echo [ERROR] Build failed: %%S [%%C^|%%P]
                set "%RESULT_PREFIX%!BUILD_COUNT!=!COLOR_RED!Failed    | %%~nS [%%C | %%P]!COLOR_RESET!"
                set "HAS_FAILED=1"
            ) else (
                echo [SUCCESS] Build succeeded: %%S [%%C^|%%P]
                set "%RESULT_PREFIX%!BUILD_COUNT!=!COLOR_GREEN!Succeeded | %%~nS [%%C | %%P]!COLOR_RESET!"
            )
            set /a "BUILD_COUNT+=1"
            echo.
        )
    )
)

@REM 빌드 결과 요약 출력
echo ==================================================
echo Build Summary:
echo.
echo   Status  ^| Project
echo ----------^|--------------------------------------

@REM 결과 배열 출력
for /l %%i in (0,1,!BUILD_COUNT!) do (
    if defined %RESULT_PREFIX%%%i (
        echo !%RESULT_PREFIX%%%i!
    )
)

echo ==================================================

@REM 실패 여부 검사
if %HAS_FAILED% EQU 1 (
    echo !COLOR_RED![SUMMARY] 일부 빌드가 실패했습니다.!COLOR_RESET!
    goto cleanup_failed
) else (
    echo !COLOR_GREEN![SUMMARY] 모든 빌드가 성공적으로 완료되었습니다.!COLOR_RESET!
    goto cleanup_success
)


:cleanup_success
if "!PAUSE_DISABLED!" neq "1" (
    pause
)
endlocal
exit /b 0

:cleanup_failed
if "!PAUSE_DISABLED!" neq "1" (
    pause
)
endlocal
exit /b 1
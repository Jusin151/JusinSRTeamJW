@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

:: 현재 디렉토리에서 모든 .cpp 및 .h 파일을 변환
for %%F in (*.cpp *.h) do (
    echo 변환 중: %%F
    powershell -Command "Get-Content '%%F' | Set-Content -Encoding UTF8 '%%F.utf8'"
    move /y "%%F.utf8" "%%F" >nul
)

echo 모든 파일 변환 완료!
pause
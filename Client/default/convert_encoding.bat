@echo off
chcp 65001 > nul  // UTF-8 코드 페이지 설정

if "%~1"=="" (
    echo 사용법: convert_encoding.bat ^<디렉토리_경로^>
    echo.
    echo 예시: convert_encoding.bat "C:\MyProject\Source"
    echo.
    echo 주의: 이 스크립트는 파일을 덮어씁니다. 중요 파일은 백업하세요.
    goto :eof
)

set target_dir="%~1"
if not exist "%target_dir%" (
    echo 오류: 디렉토리 "%target_dir%" 을(를) 찾을 수 없습니다.
    goto :eof
)

echo.
echo 지정된 디렉토리: "%target_dir%"
echo C++ (.cpp, .h) 파일들을 UTF-8로 변환 시작...
echo.

for /r "%target_dir%" %%F in (*.cpp, *.h) do (
    echo.
    echo 파일: "%%F" 변환 시도...

    // PowerShell을 사용하여 인코딩 변환 (기존 인코딩: ANSI 우선 시도, 실패 시 CP949 시도)
    powershell -NoProfile -ExecutionPolicy Bypass -Command ^
        "try { ^
            $content = Get-Content -Path '%%F' -Encoding ANSI -ErrorAction Stop; ^
            Out-File -FilePath '%%F' -Encoding UTF8 -InputObject $content -Force -NoNewline; ^
            Write-Host '성공: UTF-8 변환 완료 (원래 인코딩: ANSI)' -ForegroundColor Green; ^
        } catch { ^
            Write-Warning '경고: UTF-8 변환 실패. ANSI 디코딩 오류 또는 기타 문제 발생. 다음 인코딩 시도: CP949'; ^
            try { ^
                $content = Get-Content -Path '%%F' -Encoding CP949 -ErrorAction Stop; ^
                Out-File -FilePath '%%F' -Encoding UTF8 -InputObject $content -Force -NoNewline; ^
                Write-Host '성공: UTF-8 변환 완료 (원래 인코딩: CP949)' -ForegroundColor Green; ^
            } catch { ^
                Write-Warning '경고: UTF-8 변환 실패. CP949 디코딩 오류 또는 기타 문제 발생. 변환 건너뜁니다.'; ^
                Write-Warning '오류 메시지: $_'; ^
            } ^
        }"

    if errorlevel 1 (
        echo 오류: "%%F" 파일 변환 중 PowerShell 스크립트 오류 발생.
    )
)

echo.
echo UTF-8 인코딩 변환 완료.
echo.
pause
goto :eof
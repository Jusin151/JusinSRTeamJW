@echo off
for %%f in (*.wav) do (
  for /f "delims=_ tokens=1" %%a in ("%%f") do (
    if not exist "%%a" mkdir "%%a"
    move "%%f" "%%a\"
  )
)
echo 작업이 완료되었습니다.
pause
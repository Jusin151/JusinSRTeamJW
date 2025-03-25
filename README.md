# JusinSR
## SR프로젝트 TeamJW

## 주의사항
    주의할점 리소스 사용할때 bin/Resource사용을 피하기
    생성된 Bin 폴더는 무시되기 때문에 리소스를 직접 추가 해줘야함
    현재 수정하고 있는 헤더가 EngineSDK 인지 Engine인지 확인하기

## 자동 dll 생성 배치파일 실행
엔진 속성에 빌드후 이벤트에
```
CD $(SolutionDir)
CALL $(SolutionDir)\UpdateLib.bat
CALL $(SolutionDir)\UpdateFMODLib.bat
```
적어주세요

## 윈도우 창이 움직이지 않는다면

Client.cpp의
```
case WM_SYSCOMMAND:
    {
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    }
``` 주석처리 해주세요


## 빌드 속도 높이기

프로젝트 - 속성 - C/C++ 다중 프로세서 컴파일 예(/MP)로 바꾸기

도구 - 옵션 - 빌드 및 실행 - 최대 병렬 프로젝트 빌드 수 자기 CPU 쓰레드 수로 맞추기


## References
https://www.microsoft.com/en-us/download/details.aspx?id=8109
https://github.com/nlohmann/json
https://github.com/ocornut/imgui
https://www.fmod.com/
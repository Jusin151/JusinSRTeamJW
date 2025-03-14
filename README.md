# JusinSR

SR프로젝트 TeamJW
생성된 Bin 폴더는 무시되기 때문에 리소스를
직접 추가 해줘야함

주의할점 리소스 사용할때 bin/Resource사용을 피하기

## 자동 dll 생성 배치파일 실행
엔진 속성에 빌드후 이벤트에
```
CD $(SolutionDir)
CALL $(SolutionDir)\UpdateLib.bat
CALL $(SolutionDir)\UpdateFMODLib.bat
```
적어주세요
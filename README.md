# JusinSR

SR프로젝트 TeamJW
생성된 Bin 폴더는 무시되기 때문에 리소스를
직접 추가 해줘야함

엔진 속성에 빌드후 이벤트에
CD $(SolutionDir)
CALL $(SolutionDir)\UpdateLib.bat
CALL $(SolutionDir)\UpdateFMODLib.bat
적어주세요
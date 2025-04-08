#pragma once
#include "Base.h"
#include "Layer.h"
#include "GameObject.h"
#include "Object_Manager.h"
#include "GameInstance.h"

BEGIN(Engine)
class ENGINE_DLL CEditor final : public CBase
{
public:
    CEditor();
    virtual ~CEditor() = default;

public:
    // 오브젝트 등록
    void RegisterObject(_uint iLevelID);

    // 레벨별 저장
    _bool SaveLevel(_uint iLevelID, const _wstring& filepath);

    _bool LoadLevel(_uint iLevelID, const _wstring& filepath);

    void RenderUI()
    {
        /*  ImGui::Begin("Editor");

          static char filepath[256] = "../Save/LEVEL.json";
          static int currentLevel = 1;
          static char specificLayer[128] = "";
          ImGui::InputText("File Path", filepath, sizeof(filepath));
          ImGui::InputInt("Level ID", &currentLevel);
          if (ImGui::Button("Save Level"))
          {
              if (SaveLevel(currentLevel, ISerializable::Utf8ToWide(filepath)))
              {
                  int a = 0;
              }
          }

          if (ImGui::Button("Load Level"))
          {
              if (LoadLevel(currentLevel, ISerializable::Utf8ToWide(filepath)))
              {
                  MessageBox(nullptr, L"레벨 로드 완료!", L"로드 성공", MB_OK);
              }
              else
              {
                  MessageBox(nullptr, L"레벨 로드 실패!", L"로드 실패", MB_OK);
              }
          }

          ImGui::End();*/

        ImGui::Begin("Editor");

        static char filepath[256] = "../Save/LEVEL_GamePlay_Test.json";
        static int currentLevel = 3;
        static char specificLayer[128] = ""; // 특정 레이어 이름을 저장할 버퍼

        ImGui::InputText("File Path", filepath, sizeof(filepath));
        ImGui::InputInt("Level ID", &currentLevel);

        // 특정 레이어 입력 필드 추가
       // ImGui::InputText("Specific Layer (빈칸=모든 레이어)", specificLayer, sizeof(specificLayer));

        if (ImGui::Button("Save Level"))
        {
            if (SaveLevel(currentLevel, ISerializable::Utf8ToWide(filepath)))
            {
                MessageBox(nullptr, L"레벨 저장 완료!", L"저장 성공", MB_OK);
            }
            else
            {
                MessageBox(nullptr, L"레벨 저장 실패!", L"저장 실패", MB_OK);
            }
        }

        ImGui::SameLine(); // 같은 줄에 배치

    /*    if (ImGui::Button("Load Level"))
        {
            wstring wSpecificLayer = L"";
            if (strlen(specificLayer) > 0)
                wSpecificLayer = ISerializable::Utf8ToWide(specificLayer);

            if (LoadLevel(currentLevel, ISerializable::Utf8ToWide(filepath), wSpecificLayer))
            {
                if (wSpecificLayer.empty())
                    MessageBox(nullptr, L"모든 레이어 로드 완료!", L"로드 성공", MB_OK);
                else
                    MessageBox(nullptr, (L"레이어 '" + wSpecificLayer + L"' 로드 완료!").c_str(), L"로드 성공", MB_OK);
            }
            else
            {
                MessageBox(nullptr, L"레벨 로드 실패!", L"로드 실패", MB_OK);
            }
        }*/

        ImGui::End();
    }
   virtual  void Free() override;

private:
    map<_uint, vector<ISerializable*>> m_LevelObjects; // 레벨별 오브젝트
    CGameInstance* m_pGameInstance = { nullptr };
};
END
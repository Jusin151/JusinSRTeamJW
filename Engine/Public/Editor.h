#pragma once
#include "Base.h"
#include "Layer.h"
#include "GameObject.h"
#include "Object_Manager.h"
#include "GameInstance.h"

BEGIN(Engine)
class CEditor final : public CBase
{
public:
    CEditor() : m_pGameInstance(CGameInstance::Get_Instance())
    {
        Safe_AddRef(m_pGameInstance);
    }
    ~CEditor()
    {
        Safe_Release(m_pGameInstance);
    }

public:
    // 오브젝트 등록
    void RegisterObject(_uint iLevelID)
    {
        auto& layerMap = m_pGameInstance->m_pObject_Manager->m_pLayers[iLevelID];

        for (auto& layer : layerMap)
        {
            auto& objList = layer.second->m_GameObjects;
            for (auto it = objList.begin(); it != objList.end(); ++it)
            {
                m_LevelObjects[iLevelID].push_back(static_cast<ISerializable*>(*it));
            }
        }
    }

    // 레벨별 저장
    _bool SaveLevel(_uint iLevelID, const _wstring& filepath)
    {
        RegisterObject(iLevelID);
        /*     json j;
             j["level"] = ilevelID;
             j["objects"] = json::array();

             for (auto* obj : m_LevelObjects[ilevelID])
             {
                 j["objects"].push_back(obj->Serialize());
             }

             ofstream file(filepath);
             if (!file.is_open()) return false;

             file << j.dump(4);
             file.close();
             return true;*/

        json j;
        j["level"] = iLevelID;
        j["layers"] = json::object();

        // 레이어별로 오브젝트 그룹화
        map<wstring, vector<json>> layerObjects;

        for (auto* obj : m_LevelObjects[iLevelID])
        {
            json objData = obj->Serialize();
            _wstring layerTag = static_cast<CGameObject*>(obj)->Get_Tag();
            layerObjects[layerTag].push_back(objData);
        }

        // 그룹화된 오브젝트를 JSON에 추가
        for (const auto& [layerTag, objects] : layerObjects)
        {
            string layerTagStr = ISerializable::WideToUtf8(layerTag);
            j["layers"][layerTagStr] = objects;
        }

        // 파일에 저장
        ofstream file(filepath);
        if (!file.is_open())
            return false;

        file << j.dump(4);
        file.close();
        return true;
    }

    _bool LoadLevel(_uint iLevelID, const _wstring& filepath, const _wstring& specificLayer = L"")
    {
        // 파일 열기
        ifstream file(filepath);
        if (!file.is_open())
        {
            //MSG_BOX(L"레벨 파일을 열 수 없습니다.");
            return false;
        }

        // JSON 파싱
        json j;
        file >> j;
        file.close();

        // 레벨 ID 확인
        if (j["level"].get<_uint>() != iLevelID)
        {
            //MSG_BOX(L"레벨 ID가 일치하지 않습니다.");
            return false;
        }

        // 레이어별 오브젝트 처리
        for (auto& [layerTag, objects] : j["layers"].items())
        {
            // 레이어 태그를 wstring으로 변환
            wstring wLayerTag = ISerializable::Utf8ToWide(layerTag);

            if (!specificLayer.empty() && wLayerTag != specificLayer)
                continue;
            for (auto& objData : objects)
            {
                // 프로토타입 태그 결정
                wstring protoTag = L"Prototype_GameObject_";

                // LayerTag에서 "Layer_" 부분 제거하고 프로토타입 태그 생성
                if (wLayerTag.find(L"Layer_") == 0)
                    protoTag += wLayerTag.substr(6); // "Layer_" 이후 부분 사용
                else
                    protoTag += wLayerTag; // 그대로 사용

                // FromPool 여부 확인
                bool bFromPool = false;
                if (objData.contains("FromPool"))
                    bFromPool = objData["FromPool"].get<bool>();

                CGameObject* pGameObject = nullptr;

                // 오브젝트 생성
                if (bFromPool)
                {
                    // 풀에서 객체 가져오기
                    if (FAILED(m_pGameInstance->Add_GameObject_FromPool(
                        iLevelID, iLevelID, wLayerTag.c_str())))
                    {
                        //MSG_BOX(L"오브젝트 풀에서 객체 생성 실패");
                        continue;
                    }

                    // 방금 생성된 객체 가져오기
                    pGameObject = m_pGameInstance->Find_Last_Object(iLevelID, wLayerTag.c_str());
                }
                else
                {
                    if (FAILED(m_pGameInstance->Add_GameObject(
                        iLevelID, protoTag, iLevelID, wLayerTag)))
                    {
                        // MSG_BOX(L"프로토타입에서 객체 생성 실패");

                      /*   if(FAILED(m_pGameInstance->Add_Prototype(iLevelID,protoTag,)))*/
                        continue;
                    }

                    // 방금 생성된 객체 가져오기
                    pGameObject = m_pGameInstance->Find_Last_Object(iLevelID, wLayerTag.c_str());
                }

                // 생성된 객체에 JSON 데이터 적용
                if (pGameObject)
                {
                    ISerializable* pSerializable = dynamic_cast<ISerializable*>(pGameObject);
                    if (pSerializable)
                        pSerializable->Deserialize(objData);
                }
            }
        }

        return true;
    }

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

        static char filepath[256] = "../Save/LEVEL.json";
        static int currentLevel = 1;
        static char specificLayer[128] = ""; // 특정 레이어 이름을 저장할 버퍼

        ImGui::InputText("File Path", filepath, sizeof(filepath));
        ImGui::InputInt("Level ID", &currentLevel);

        // 특정 레이어 입력 필드 추가
        ImGui::InputText("Specific Layer (빈칸=모든 레이어)", specificLayer, sizeof(specificLayer));

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

        if (ImGui::Button("Load Level"))
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
        }

        ImGui::End();
    }

private:
    map<_uint, vector<ISerializable*>> m_LevelObjects; // 레벨별 오브젝트
    CGameInstance* m_pGameInstance = { nullptr };
};
END
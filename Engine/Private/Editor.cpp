#include "Editor.h"
#include <regex>

CEditor::CEditor(): m_pGameInstance(CGameInstance::Get_Instance())
{
    Safe_AddRef(m_pGameInstance);
}
void CEditor::RegisterObject(_uint iLevelID)
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

_bool CEditor::SaveLevel(_uint iLevelID, const _wstring& filepath)
{
    RegisterObject(iLevelID);
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
    //file << j.dump(4);
    //file.close();

    string jsonStr = j.dump(4);
    regex floatPattern("(\\d+)\\.(\\d{1,2})\\d*");
    jsonStr = regex_replace(jsonStr, floatPattern, "$1.$2");
    file << jsonStr;
    m_LevelObjects[iLevelID].clear();
    return true;
}

_bool CEditor::LoadLevel(_uint iLevelID, const _wstring& filepath)
{
    // 1. 파일 열기 및 JSON 파싱
    ifstream file(filepath);
    if (!file.is_open())
        return false;

    json j;
    try {
        file >> j;
    }
    catch (const exception&) {
        // JSON 파싱 오류 처리
        MSG_BOX("Failed to parse level file");
        return false;
    }
    file.close();

    // 2. 레벨 ID 확인
    _uint savedLevelID = j["level"];
    if (savedLevelID != iLevelID) {
        MSG_BOX("Warning: Saved level ID differs from target level ID");
        // 계속 진행할지 결정 (여기서는 계속 진행)
    }

    // 3. 기존 레벨의 모든 오브젝트 삭제
 //   ClearLevel(iLevelID);

    // 4. 레이어별로 오브젝트 로드 및 생성
    for (const auto& [layerTagStr, objects] : j["layers"].items()) {
        // 레이어 태그를 wstring으로 변환
        wstring layerTag = ISerializable::Utf8ToWide(layerTagStr);

        // 각 오브젝트 처리
        for (const auto& objData : objects) {
            // 오브젝트 생성에 필요한 기본 정보 추출
            string classNameStr = objData["className"];
            wstring className = ISerializable::Utf8ToWide(classNameStr);

            // 프로토타입 태그 가져오기
            wstring protoTag;
            if (objData.contains("prototypeTag")) {
                string protoTagStr = objData["prototypeTag"];
                protoTag = ISerializable::Utf8ToWide(protoTagStr);
            }
            else {
                // 프로토타입 태그가 없으면 기본값 설정
                protoTag = L"Prototype_GameObject_" + className.substr(1); // "C" 접두사 제거
            }

            // 오브젝트 생성을 위한 설명 구조체
            CGameObject::OBJECT_DESC objDesc{};

            // objData에서 필요한 필드 추출
            if (objData.contains("level"))
                objDesc.iLevel = objData["level"];
            else
                objDesc.iLevel = iLevelID;

            if (objData.contains("protoLevel"))
                objDesc.iProtoLevel = objData["protoLevel"];
            else
                objDesc.iProtoLevel = objDesc.iLevel;

            // 버퍼 태그 설정
            if (objData.contains("bufferTag")) {
                string bufferTagStr = objData["bufferTag"];
                objDesc.stBufferTag = ISerializable::Utf8ToWide(bufferTagStr);
            }
            else {
                objDesc.stBufferTag = L"Prototype_Component_VIBuffer_Cube"; // 기본값
            }

            // 텍스처 태그 설정
            if (objData.contains("textureTag")) {
                string textureTagStr = objData["textureTag"];
                objDesc.stProtTextureTag = ISerializable::Utf8ToWide(textureTagStr);
            }
            else {
                objDesc.stProtTextureTag = L""; // 기본값
            }

            // 프로토타입 태그 설정
            objDesc.stProtTag = protoTag;

            // 오브젝트 생성
            CGameObject* pNewObject = nullptr;

            // 1. 프로토타입이 이미 존재하는지 확인
            if (SUCCEEDED(m_pGameInstance->Find_Prototype(protoTag)))
            {
                // 프로토타입이 있으면 해당 프로토타입으로 게임 오브젝트 추가
                if (FAILED(m_pGameInstance->Add_GameObject(objDesc.iProtoLevel, protoTag,
                    objDesc.iLevel, layerTag, &objDesc))) {
                    MSG_BOX("Failed to add game object");
                    continue;
                }
            }
           

            // 오브젝트 생성 성공, 속성 설정
            if (pNewObject != nullptr)
            {
                // ISerializable 인터페이스로 캐스트
                ISerializable* pSerializable = static_cast<ISerializable*>(pNewObject);

                // 트랜스폼 정보 설정
                if (objData.contains("transform")) {
                    // 트랜스폼 컴포넌트 가져오기
                    CTransform* pTransform = (CTransform*)pNewObject->Get_Component(TEXT("Com_Transform"));
                    if (pTransform != nullptr) {
                        // 위치 설정
                        if (objData["transform"].contains("position")) {
                            _float3 position;
                            position.x = objData["transform"]["position"][0];
                            position.y = objData["transform"]["position"][1];
                            position.z = objData["transform"]["position"][2];
                            pTransform->Set_State(CTransform::STATE_POSITION, position);
                        }

                        // 회전 설정
                        if (objData["transform"].contains("rotation")) {
                            _float3 rotation;
                            rotation.x = objData["transform"]["rotation"][0];
                            rotation.y = objData["transform"]["rotation"][1];
                            rotation.z = objData["transform"]["rotation"][2];
                            pTransform->Rotate_EulerAngles(rotation);
                        }

                        // 크기 설정
                        if (objData["transform"].contains("scale")) {
                            _float3 scale;
                            scale.x = objData["transform"]["scale"][0];
                            scale.y = objData["transform"]["scale"][1];
                            scale.z = objData["transform"]["scale"][2];
                            //pTransform->Set_Scale(scale);
                        }
                    }
                }

                // 객체별 추가 속성 역직렬화
                pSerializable->Deserialize(objData);
            }
        }
    }

    // 로드 완료 후 객체 등록
    RegisterObject(iLevelID);

    return true;
}


void CEditor::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}

#include "Editor.h"

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

    file << j.dump(4);
    file.close();
    return true;
}

void CEditor::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}

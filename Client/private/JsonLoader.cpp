#include "JsonLoader.h"
#include <Player.h>
#include <Terrain.h>
#include <TestMonster.h>
#include <Structure.h>
#include <Camera_Free.h>
#include <Camera_FirstPerson.h>
#include "UI_Headers.h" 
#include "GamePlay_Button.h"

HRESULT CJsonLoader::Load_Prototypes(CGameInstance* pGameInstance, LPDIRECT3DDEVICE9 pGraphic_Device,const _wstring& filePath)
{
    // JSON 파일 로드
    ifstream file(filePath);
    if (!file.is_open()) 
    {
        MSG_BOX("프로토타입 JSON 파일을 찾을 수 없습니다.");
        return E_FAIL;
    }

    json j;
    file >> j;

    // 텍스처 프로토타입 로드
    if (j.contains("textures")) 
    {
        for (const auto& texture : j["textures"])
        {
            _wstring tag = ISerializable::Utf8ToWide(texture["tag"]);
            _wstring path = ISerializable::Utf8ToWide(texture["path"]);
            LEVEL level = static_cast<LEVEL>(texture["level"].get<_uint>());
            _uint count = texture["count"];
            if (FAILED(pGameInstance->Add_Prototype(level, tag,
                CTexture::Create(pGraphic_Device, CTexture::TYPE_2D, path.c_str(), count))))
                return E_FAIL;
        }
    }

    // 게임 오브젝트 프로토타입 로드
    if (j.contains("gameObjects")) 
    {
        for (const auto& obj : j["gameObjects"])
        {
            _wstring tag = ISerializable::Utf8ToWide(obj["tag"]);
            LEVEL level = static_cast<LEVEL>(obj["level"].get<_uint>());
            string className = obj["class"];
            CBase* pGameObject = Create_Object_ByClassName(className, pGraphic_Device);
            if (!pGameObject)
                continue; 

            if (FAILED(pGameInstance->Add_Prototype(level, tag, pGameObject)))
                return E_FAIL;
        }
    }

    // 버퍼 프로토타입 로드
    if (j.contains("buffers")) 
    {
        for (const auto& buffer : j["buffers"])
        {
            _wstring tag = ISerializable::Utf8ToWide(buffer["tag"]);
            LEVEL level = static_cast<LEVEL>(buffer["level"].get<_uint>());
            string className = buffer["class"];

            // 버퍼 생성 로직 추가
            CBase* pBuffer = nullptr;

            if (className == "CVIBuffer_Terrain") 
            {
                _uint width = buffer["width"].get<_uint>();
                _uint height = buffer["height"].get<_uint>();
                pBuffer = CVIBuffer_Terrain::Create(pGraphic_Device, width, height);
            }
            // 다른 타입의 버퍼가 있다면 여기에 추가

            if (!pBuffer)
                continue;

            if (FAILED(pGameInstance->Add_Prototype(level, tag, pBuffer)))
                return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CJsonLoader::Load_Level(CGameInstance* pGameInstance, LPDIRECT3DDEVICE9 pGraphic_Device, const _wstring& jsonFilePath, LEVEL eLevel)
{
    ifstream file(jsonFilePath);
    if (!file.is_open())
    {
        MSG_BOX("레벨 JSON 파일을 찾을 수 없습니다.");
        return E_FAIL;
    }

    json j;
    file >> j;

    for (const auto& [layerName, layerObjects] : j["layers"].items())
    {
        _wstring wLayerName = ISerializable::Utf8ToWide(layerName);

        if (wLayerName == L"Layer_Structure")
        {
            for (const auto& objData : layerObjects)
            {
                CStructure::STRUCTURE_DESC tDesc{};

                wstring textureTag, vIBuffer, colliderTag;

                if (objData.contains("texture_tag"))
                    textureTag = ISerializable::Utf8ToWide(objData["texture_tag"].get<string>());

                if (objData.contains("vibuffer"))
                    vIBuffer = ISerializable::Utf8ToWide(objData["vibuffer"].get<string>());

                if (objData.contains("collider_tag"))
                    colliderTag = ISerializable::Utf8ToWide(objData["collider_tag"].get<string>());

                if (objData.contains("texture_path"))
                    tDesc.stTexturePath = ISerializable::Utf8ToWide(objData["texture_path"].get<string>());

                tDesc.stTextureTag = textureTag.empty() ? nullptr : textureTag.c_str();
                tDesc.stVIBuffer = vIBuffer.empty() ? nullptr : vIBuffer.c_str();
                tDesc.stCollProtoTag = colliderTag.empty() ? nullptr : colliderTag.c_str();

                if (objData.contains("transform") && objData["transform"].size() >= 1)
                {
                    auto& posData = objData["transform"][0];
                    if (posData[0] == "position")
                    {
                        tDesc.vPos.x = posData[1].get<float>();
                        tDesc.vPos.y = posData[2].get<float>();
                        tDesc.vPos.z = posData[3].get<float>();
                    }
                }

                if (FAILED(pGameInstance->Add_GameObject(eLevel,
                    TEXT("Prototype_GameObject_Structure"),
                    eLevel,
                    wLayerName,
                    &tDesc)))
                    continue;

                CGameObject* pGameObject = pGameInstance->Find_Last_Object(eLevel, wLayerName);
                if (pGameObject)
                    pGameObject->Deserialize(objData);
            }
        }
        else
        {
            //// Existing code for other layers...
            //for (const auto& objData : layerObjects)
            //{
            //    // FromPool handling code
            //    // Prototype determination code
            //    // Rest of your existing implementation
            //}
        }
    }

    return S_OK;

}

CBase* CJsonLoader::Create_Object_ByClassName(const string& className, LPDIRECT3DDEVICE9 pGraphic_Device)
{
    // 기본 게임 오브젝트
    if (className == "CPlayer")
        return CPlayer::Create(pGraphic_Device);
    else if (className == "CTestMonster")
        return CTestMonster::Create(pGraphic_Device);
    else if (className == "CTerrain")
        return CTerrain::Create(pGraphic_Device);
    else if (className == "CStructure")
        return CStructure::Create(pGraphic_Device);
    // 카메라
    else if (className == "CCamera_Free")
        return CCamera_Free::Create(pGraphic_Device);
    else if (className == "CCamera_FirstPerson")
        return CCamera_FirstPerson::Create(pGraphic_Device);
    // UI 컴포넌트
    else if (className == "CUI_Default_Panel")
        return CUI_Default_Panel::Create(pGraphic_Device);
    else if (className == "CUI_Left_Display")
        return CUI_Left_Display::Create(pGraphic_Device);
    else if (className == "CUI_Player_Icon")
        return CUI_Player_Icon::Create(pGraphic_Device);
    else if (className == "CUI_HP_Bar")
        return CUI_HP_Bar::Create(pGraphic_Device);
    else if (className == "CUI_MP_Bar")
        return CUI_MP_Bar::Create(pGraphic_Device);
    else if (className == "CUI_Mid_Display")
        return CUI_Mid_Display::Create(pGraphic_Device);
    else if (className == "CUI_Right_Display")
        return CUI_Right_Display::Create(pGraphic_Device);
    else if (className == "CUI_Bullet_Bar")
        return CUI_Bullet_Bar::Create(pGraphic_Device);
    else if (className == "CUI_Menu")
        return CUI_Menu::Create(pGraphic_Device);
    else if (className == "CGamePlay_Button")
        return CGamePlay_Button::Create(pGraphic_Device);
    else if (className == "CUI_Spell_Shop")
        return CUI_Spell_Shop::Create(pGraphic_Device);

    // 찾지 못한 클래스 이름에 대한 처리
    wstring wClassName = ISerializable::Utf8ToWide(className);
    wstring errorMsg = L"알 수 없는 클래스 이름: " + wClassName;
    OutputDebugString(errorMsg.c_str());

    return nullptr;
}

_wstring CJsonLoader::Get_Prototype_For_Layer(const _wstring& layerName)
{
    if (layerName == L"Layer_Player")
        return L"Prototype_GameObject_Player";
    else if (layerName == L"Layer_Monster")
        return L"Prototype_GameObject_TestMonster";
    else if (layerName == L"Layer_BackGround")
        return L"Prototype_GameObject_Terrain";
    else if (layerName == L"Layer_Camera")
        return L"Prototype_GameObject_Camera_FirstPerson";
    else if (layerName == L"Layer_Default_PlayerUI")
        return L"Prototype_GameObject_Default_PlayerUI";

    return L"";
}

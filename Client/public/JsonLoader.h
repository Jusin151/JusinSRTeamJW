#pragma once
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Serializable.h"
class CJsonLoader final : public CBase
{
public:
    HRESULT Load_Prototypes(CGameInstance* pGameInstance, LPDIRECT3DDEVICE9 pGraphic_Device, const _wstring& filePath);
    HRESULT Load_Level(CGameInstance* pGameInstance, LPDIRECT3DDEVICE9 pGraphic_Device, const _wstring& jsonFilePath, LEVEL eLevel);
    CBase* Create_Object_ByClassName(const string& className, LPDIRECT3DDEVICE9 pGraphic_Device);
    HRESULT LoadClassNamesFromJson(const string& filePath, vector<string>& outClassNames);
    _int CountPrototypes(const _wstring& filePath) const;
    HRESULT Load_Prototypes(
        CGameInstance* pGameInstance,
        LPDIRECT3DDEVICE9 pGraphic_Device,
        const _wstring& filePath,
        function<void()> onEntryLoaded 
    );

};


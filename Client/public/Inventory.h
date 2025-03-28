#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Weapon_Base.h"
#include "Item_Manager.h"
#include "Inven_UI.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;  // UI는 사각형으로 할듯
END


BEGIN(Client)

class CInventory final : public CGameObject
{
    typedef struct tagInven_INFO
    {
        _float2 vPos;
        _float2 vSize;
    }Inven_DESC;

public:
    CInventory(LPDIRECT3DDEVICE9 pGraphic_Device);
    CInventory(const CInventory& Prototype);
    virtual ~CInventory() = default;

public:
     HRESULT Initialize_Prototype()override;
     HRESULT Initialize(void* pArg)override;
     void Priority_Update(_float fTimeDelta)override;
     void Update(_float fTimeDelta)override;
     void Late_Update(_float fTimeDelta)override;
     HRESULT Render()override;
private:
    HRESULT Ready_Components();
   
public:
    CWeapon_Base* Equip(_uint type);


public:
    static CInventory* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free();


    void Add_Weapon(wstring tag, _uint Index)
    {
        Index -= 1;
        // 인벤 공간은 최대 7개 
        if (m_vecpItem.size() < Index)
        {
            MSG_BOX("무기 추가할때 인덱스 신경써주세욤");
            return;
        }
        m_vecpItem[Index]=CItem_Manager::GetInstance()->Get_Weapon(tag);
        m_pInven_UI->Add_WeaponIcon(tag);
    }
    CWeapon_Base* Get_Weapon(const wstring& tag)
    {
        auto it = m_WeaponMap.find(tag);
        if (it != m_WeaponMap.end())
        {
            return it->second;
        }
        return nullptr;
    }
    CWeapon_Base* Weapon_Equip(_uint Index)
    {
    
        for (auto& pItem : m_vecpItem)
        {
            if (pItem&&pItem->IsActive()) //
            {
                pItem->SetActive(false);
                break;
            }
        }

        // 끼려고 하는 아이템 껴보리기
       
        auto it = m_vecpItem[Index];
        if (it != nullptr)
        {
            it->SetActive(true);
            m_pInven_UI->WeaponIcon_isActive(Index);
            return it;

        }
        return nullptr; //안껴지는게 아니라 터지는건가?
    }

protected:
    CTexture* m_pTextureCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CTransform* m_pTransformCom = { nullptr };
private:

    unordered_map<wstring, CWeapon_Base*> m_WeaponMap;
     CWeapon_Base* m_pItem = { nullptr };
     CInven_UI* m_pInven_UI = { nullptr };
     vector<CWeapon_Base*> m_vecpItem{ 8, nullptr };
private:
     Inven_DESC m_Inven_INFO{};
     _bool m_bRender{};
    _bool m_bFrist_off_Item{};
    _float fElapsedTime{};
    _bool bFirstUpdate = {};
    _bool m_bFristInit = {};
    _float m_fNoInputAccTime = 0.f;
    _bool bInputReceived = {};
    _bool m_bKeyPressed = { false };
};
END
#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Weapon_Base.h"

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
    virtual HRESULT Initialize_Prototype()override;
    virtual HRESULT Initialize(void* pArg)override;
    virtual void Priority_Update(_float fTimeDelta)override;
    virtual void Update(_float fTimeDelta)override;
    virtual void Late_Update(_float fTimeDelta)override;
    virtual HRESULT Render()override;


private:
    HRESULT Ready_Components();

public:
    static CInventory* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free();


    void Add_Weapon(const wstring& tag, CWeapon_Base* pUI)
    {
        if (m_UIMap.find(tag) != m_UIMap.end())
        {
            return;
        }
        m_UIMap[tag] = pUI;
    }

    CWeapon_Base* Get_Weapon(const wstring& tag)
    {
        auto it = m_UIMap.find(tag);
        if (it != m_UIMap.end())
        {
            return it->second;
        }
        return nullptr;
    }

protected:
    CTexture* m_pTextureCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CTransform* m_pTransformCom = { nullptr };
    Inven_DESC m_Inven_INFO{};

    unordered_map<wstring, CWeapon_Base*> m_UIMap;

    CWeapon_Base* m_pItem{};

    _bool m_bFrist_off_Item{};
};
END
#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Item_Manager.h"
#include "Inven_UI.h"
#include "Ranged_Weapon.h"
#include "Melee_Weapon.h"
#include "Item.h"
#include "Observer.h"

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


    void Add_Weapon(const _wstring& tag, _uint Index)
    {
        Index -= 1;
        // 인벤 공간은 최대 7개 
        if (m_vecpWeapons.size() < Index)
        {
            MSG_BOX("무기 추가할때 인덱스 신경써주세욤");
            return;
        }
        m_vecpWeapons[Index]=CItem_Manager::GetInstance()->Get_Weapon(tag);
        m_WeaponMap[tag] = m_vecpWeapons[Index];

        _wstring ptag = tag;  
		pair<const _wstring&, _uint> testTag = { tag, Index };
        Notify((void*)(&testTag), L"AddWeaponIcon"); // AddWeapon

        Add_Item(tag);  
    }
    void Add_Item(const _wstring& tag)
    {
        if (!Exist_item(tag))
        {
            m_MapItem[tag] = true;
        }
    }
    _bool Exist_item(const _wstring& tag)
    {
		auto it = m_MapItem.find(tag);
		if (it != m_MapItem.end())
		{
			return true;
		}
		return false;
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
    CWeapon_Base* Get_Item(const wstring& tag)
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
    
        for (auto& pItem : m_vecpWeapons)
        {
            if (pItem&&pItem->IsActive()) //
            {
                pItem->SetActive(false);
                break;
            }
        }

        // 끼려고 하는 아이템 껴보리기
       
        auto it = m_vecpWeapons[Index];
        if (it != nullptr)
        {
            it->SetActive(true);

           //m_pInven_UI->WeaponIcon_isActive(Index);
			Notify(&Index, L"IconActive");

            if (CRanged_Weapon* pRanged = dynamic_cast<CRanged_Weapon*>(it))
            {
                pRanged->Notify_Bullet();
            }
            else if (CMelee_Weapon* pMeleed = dynamic_cast<CMelee_Weapon*>(it))
            {
                pMeleed->Notify_Bullet();
            }
            else
                return nullptr;
             
            return it;

        }
        return nullptr; 
    }
private:
	vector<CObserver*> m_pObservers{};
public:
	void Add_Observer(CObserver* pObserver)
	{
		m_pObservers.push_back(pObserver);
	}
	virtual void Notify(void* pArg, const wstring& type)
	{
		for (auto& obs : m_pObservers)
			obs->OnNotify(pArg, type);
	}
protected:
    CTexture* m_pTextureCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CTransform* m_pTransformCom = { nullptr };
private:

    unordered_map<wstring, CWeapon_Base*> m_WeaponMap{};
     CWeapon_Base* m_pItem = { nullptr };
    // CInven_UI* m_pInven_UI = { nullptr };
      vector<CWeapon_Base*> m_vecpWeapons{ 8, nullptr };
	 unordered_map<wstring, _bool> m_MapItem; //
private:
     Inven_DESC m_Inven_INFO{};
     _bool m_bRender{};
    _bool m_bFrist_off_Item{};
    _float fElapsedTime{};
    _bool bFirstUpdate = {};
    _bool m_bFristInit = {};
    _float m_fNoInputAccTime = { 0.f };
    _bool bInputReceived = {};
    _bool m_bKeyPressed = { false };
private:

};
END
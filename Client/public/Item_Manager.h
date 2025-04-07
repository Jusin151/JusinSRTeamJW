#pragma once

#include "Client_Defines.h"
#include "Weapon_Base.h"
#include "Item.h"
#include "Melee_Weapon.h"
#include "Ranged_Weapon.h"

BEGIN(Client)
class CItem_Manager 
{
public:
    static CItem_Manager* GetInstance()
    {
        static CItem_Manager instance;
        return &instance;
    }

private:
    CItem_Manager() = default;

    CItem_Manager(const CItem_Manager&) = delete;
    CItem_Manager& operator=(const CItem_Manager&) = delete;
    CItem_Manager(CItem_Manager&&) = delete;
    CItem_Manager& operator=(CItem_Manager&&) = delete;

public:


    void Add_Weapon(const wstring& tag, CWeapon_Base* pUI)
    {
        if (m_MapItem.find(tag) != m_MapItem.end())
        {
            return;
        }
        m_MapItem[tag] = pUI;
    }
    void SetUp_MeleeWeapon_to_Strength(_int str)
    {
        if (m_MapItem[L"Axe"] == nullptr || m_MapItem[L"Claymore"] == nullptr)
            return;
        m_MapItem[L"Axe"]->Set_Ap(str);
        m_MapItem[L"Claymore"]->Set_Ap(str);
    }
    void SetUp_RangedWeapon_to_Capacity(_int Capacity)
    {
        Set_MaxBullet_if_Ranged(L"Magnum", Capacity);
        Set_MaxBullet_if_Ranged(L"ShotGun", Capacity);
        Set_MaxBullet_if_Ranged(L"Minigun", Capacity);
        Set_MaxBullet_if_Ranged(L"Harvester", Capacity);
    }

    void Set_MaxBullet_if_Ranged(const wstring& weaponTag, _int capacity)
    {
        auto it = m_MapItem.find(weaponTag);
        if (it == m_MapItem.end() || it->second == nullptr)
            return;

      
        if (auto pRanged = dynamic_cast<CRanged_Weapon*>(it->second))
        {
            pRanged->Ranged_INFO.MaxAmmo += capacity; 
            pRanged->Ranged_INFO.CurrentAmmo += capacity;
            pRanged->Notify_Bullet(); // UI 갱신 
        }
    }



    CWeapon_Base* Get_Weapon(const wstring& tag)
    {
        auto it = m_MapItem.find(tag);
        if (it != m_MapItem.end())
        {
            return it->second;
        }
        return nullptr;
    }
    CWeapon_Base* Weapon_Equip(const wstring& tag)
    {
        //원래 끼고 있던거 해제
        for (auto& pair : m_MapItem)
        {
            if (pair.second->IsActive())
            {
                pair.second->SetActive(false);
                break;
            }
        }
        // 끼려고 하는 아이템 껴보리기
        auto it = m_MapItem.find(tag);
        if (it != m_MapItem.end())
        {
            it->second->SetActive(true);
          
            return it->second;
        }
        return nullptr;
    }
    void Weapon_UnEquip()
    {
        //원래 끼고 있던거 해제
        for (auto& pair : m_MapItem)
                pair.second->SetActive(false);  
    }

    void All_Weapon_Off()
    {

        for (auto& pair : m_MapItem)
        {
            if (pair.second->IsActive())
            {
                pair.second->SetActive(false);
                break;
            }
        }
    }

    void Set_Inven_Render(_bool type){ Render_off = type;}
    _bool Get_Inven_Render() { return Render_off; }
private:
    unordered_map<wstring, CWeapon_Base*> m_MapItem;

    _bool Render_off{};
};
END



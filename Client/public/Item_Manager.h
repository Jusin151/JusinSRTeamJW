#pragma once

#include "Client_Defines.h"
#include "Weapon_Base.h"
#include "Item_Icon.h"

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
        if (m_UIMap.find(tag) != m_UIMap.end())
        {
            return;
        }
        m_UIMap[tag] = pUI;
    }
    void Add_Icon(const wstring& tag, CItem_Icon* pUI)
    {
        if (m_Weapon_Icon.find(tag) != m_Weapon_Icon.end())
        {
            return;
        }
        m_Weapon_Icon[tag] = pUI;
    }
    void Set_Select_Icon(const wstring& tag, CItem_Icon* pUI)
    {
        for (auto& pair : m_Weapon_Icon)
        {
           
                pair.second->Set_Base_Size();
                break;
           
        }

        auto it = m_Weapon_Icon.find(tag);
        if (it != m_Weapon_Icon.end())
        {
            it->second->Set_Select();
        }
    }

    //혹시 몰라서 만들었음
    CWeapon_Base* Get_Weapon(const wstring& tag)
    {
        auto it = m_UIMap.find(tag);
        if (it != m_UIMap.end())
        {
            return it->second;
        }
        return nullptr;
    }


    CWeapon_Base* Weapon_Equip(const wstring& tag)
    {
        //원래 끼고 있던거 해제
        for (auto& pair : m_UIMap)
        {
            if (pair.second->IsActive())
            {
                pair.second->SetActive(false);
                break;
            }
        }

        // 끼려고 하는 아이템 껴보리기
        auto it = m_UIMap.find(tag);
        if (it != m_UIMap.end())
        {
            it->second->SetActive(true);
            return it->second;
        }
        return nullptr;
    }

    void Weapon_UnEquip()
    {
        //원래 끼고 있던거 해제
        for (auto& pair : m_UIMap)
        {   
                pair.second->SetActive(false);
            
        }
      
    }

  

private:
    unordered_map<wstring, CWeapon_Base*> m_UIMap;
    unordered_map<wstring, CItem_Icon*> m_Weapon_Icon;
};
END
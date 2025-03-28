#pragma once

#include "Client_Defines.h"
#include "Weapon_Base.h"


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
        if (m_ItemMap.find(tag) != m_ItemMap.end())
        {
            return;
        }
        m_ItemMap[tag] = pUI;
    }

    CWeapon_Base* Get_Weapon(const wstring& tag)
    {
        auto it = m_ItemMap.find(tag);
        if (it != m_ItemMap.end())
        {
            return it->second;
        }
        return nullptr;
    }
    CWeapon_Base* Weapon_Equip(const wstring& tag)
    {
        //원래 끼고 있던거 해제
        for (auto& pair : m_ItemMap)
        {
            if (pair.second->IsActive())
            {
                pair.second->SetActive(false);
                break;
            }
        }

        // 끼려고 하는 아이템 껴보리기
        auto it = m_ItemMap.find(tag);
        if (it != m_ItemMap.end())
        {
            it->second->SetActive(true);
          
            return it->second;
        }
        return nullptr;
    }
    void Weapon_UnEquip()
    {
        //원래 끼고 있던거 해제
        for (auto& pair : m_ItemMap)
                pair.second->SetActive(false);  
    }

    _bool Get_Current_Weapon_Active(const wstring& tag)
    {

        auto it = m_ItemMap.find(tag);
        if (it == m_ItemMap.end()) // 동완아 반환하는 함수는 꼭 이렇게 다 반환하도록
        {
            return false;  
        }
     
        return it->second->IsActive();
    }
    void All_Weapon_Off()
    {

        for (auto& pair : m_ItemMap)
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
    unordered_map<wstring, CWeapon_Base*> m_ItemMap;

    _bool Render_off{};
};
END
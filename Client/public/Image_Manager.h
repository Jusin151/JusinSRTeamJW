#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Image.h"
#include "Item_Manager.h"
#include <GameInstance.h>

class CImage_Manager
{
    enum ICON_NUM
    {

        Claymore,Axe,Magnum,ShotGun,Staff,Minigun,Harvester

    };
 public:
    static CImage_Manager* GetInstance()
    {
        static CImage_Manager instance;
        return &instance;
    }

private:
    CImage_Manager() = default;
    CImage_Manager(const CImage_Manager&) = delete;
    CImage_Manager& operator=(const CImage_Manager&) = delete;
    CImage_Manager(CImage_Manager&&) = delete;
    CImage_Manager& operator=(CImage_Manager&&) = delete;
public:

    void Add_Image(const wstring& tag, CImage* pUI)
    {
        if (m_mapImage.find(tag) != m_mapImage.end())
        {
            return;
        } 
        m_mapImage[tag] = pUI;
    }
    void Add_Weapon_Icon(const wstring& tag,CImage* IconImage)//무기이름과 this
    {    
        if (m_mapWeaponIcon.find(tag) != m_mapWeaponIcon.end())
        {
            return; 
        }
        m_mapWeaponIcon[tag] = IconImage;
    }
    void Weapon_Icon_Init()//이거 안해주면 아이콘들보임
    {
        for (auto& it : m_mapWeaponIcon)
        {
            it.second->SetActive(false);
        }
    }
    CImage* Get_Image(const wstring& tag)
    {
        auto it = m_mapImage.find(tag);
        if (it != m_mapImage.end())
        {
            return it->second;
        }
        return nullptr;
    }
    CImage* Get_WeaponIcon(const wstring& tag)
    {
        auto it = m_mapWeaponIcon.find(tag);
        if (it != m_mapWeaponIcon.end())
        {
            return it->second;
        }
        return nullptr;
    }

private:
    unordered_map<wstring, CImage*> m_mapImage={};
    unordered_map<wstring, CImage*> m_mapWeaponIcon={};
 
};



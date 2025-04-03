#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "UI_Headers.h"

class CUI_Manager
{
public:
    static CUI_Manager* GetInstance()
    {
        static CUI_Manager instance;
        return &instance;
    }

private:
    CUI_Manager() = default;


    CUI_Manager(const CUI_Manager&) = delete;
    CUI_Manager& operator=(const CUI_Manager&) = delete;
    CUI_Manager(CUI_Manager&&) = delete;
    CUI_Manager& operator=(CUI_Manager&&) = delete;

public:
    void AddUI(const wstring& tag, CUI_Base* pUI)
    {
        if (m_UIMap.find(tag) != m_UIMap.end())
        {
            return;
        }
        m_UIMap[tag] = pUI;
    }

    CUI_Base* GetUI(const wstring& tag)
    {
        auto it = m_UIMap.find(tag);
        if (it != m_UIMap.end())
        {
            return it->second;
        }
        return nullptr;
    }


    _float2 GetParent_Pos()
    {
        CUI_Base* pUI = GetUI(L"Parent");
        if (pUI)
        {
            return pUI->Get_Position();
        }
        return _float2(99999, 99999);
    }
    _float2 GetLeftPanel_Pos()
    {
        CUI_Base* pUI = GetUI(L"Left_Panel");
        if (pUI)
        {
            return pUI->Get_Position();
        }
        return _float2(99999, 99999);
    }
    _float2 GetRightPanel_Pos()
    {
        CUI_Base* pUI = GetUI(L"Right_Panel");
        if (pUI)
        {
            return pUI->Get_Position();
        }
        return _float2(99999, 99999);
    }
    _float2 GetMidPanel_Pos()
    {
        CUI_Base* pUI = GetUI(L"Mid_Panel");
        if (pUI)
        {
            return pUI->Get_Position();
        }
        return _float2(99999, 99999);
    }
    void Init_HP_UI(_uint CurrentHP,_uint MaxHP)
    {
        CUI_Base* pHp_UI = GetUI(L"Hp_Bar");
        static_cast<CUI_HP_Bar*>(pHp_UI)->Init_HP(CurrentHP, MaxHP);
    }
    void Init_Exp_UI(_uint CurrentExp, _uint MaxExp)
    {
        CUI_Base* pExp_UI = GetUI(L"Exp_Bar");
        static_cast<CUI_Exp_Bar*>(pExp_UI)->Init_EXP(CurrentExp, MaxExp);
    }


    _uint Get_Hp()
    {
        CUI_Base* pUI = GetUI(L"Hp_Bar");

        return static_cast<CUI_HP_Bar*>(pUI)->Get_Health();
    }

    void Set_MP(_uint type)
    {
        CUI_Base* pUI = GetUI(L"MP_Bar");

        static_cast<CUI_MP_Bar*>(pUI)->Set_MP(type);
    }


    void Set_Minigun_Bullet(_uint type)
    {
        CUI_Base* pUI = GetUI(L"Bullet_Bar");

        static_cast<CUI_Bullet_Bar*>(pUI)->Set_Bullet(type);
    }



private:
    unordered_map<wstring, CUI_Base*> m_UIMap;
 

};



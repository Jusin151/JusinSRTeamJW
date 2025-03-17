#pragma once

#include "Client_Defines.h"
#include "CUI_Base.h"
#include <vector>
#include <unordered_map>
#include <string>

BEGIN(Client)

class CItemBase;

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


    // 아이템 추가, 제거, 조회 기능
    void AddItem(CItemBase* pItem) 
    {

    }
    void RemoveItem(int iItemID)
    {

    }
    CItemBase* GetItem(int iItemID)
    {

    }
    const vector<CItemBase*>& GetAllItems() const { return m_Items; }



private:
    vector<CItemBase*> m_Items;
    unordered_map<int, CItemBase*> m_ItemMap; // 아이템 ID를 키로 관리

    // 인벤토리 UI 관련 데이터 (예: 슬롯 위치, 배경 이미지 등)을 추가할 수 있음.
};
END

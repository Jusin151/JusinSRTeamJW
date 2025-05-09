﻿#pragma once
#include "Shop.h"
#include "Observer.h"

BEGIN(Client)
class CHub_SpellShop : public CShop
{
private:
    CHub_SpellShop(LPDIRECT3DDEVICE9 pGraphic_Device);
    CHub_SpellShop(const CHub_SpellShop& Prototype);
    virtual ~CHub_SpellShop() = default;

public:
    // 기본 상속 함수
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

    virtual void Priority_Update(_float fTimeDelta)override;
    virtual void Update(_float fTimeDelta)override;
    virtual void Late_Update(_float fTimeDelta)override;
    virtual HRESULT Render()override;

    // Shop 인터페이스 구현

    virtual HRESULT Open_Shop() override;
    virtual HRESULT Close_Shop() override;
    HRESULT SetUp_RenderState();
    HRESULT Release_RenderState();


protected:
    virtual HRESULT Ready_ShopItems() override;
private:
    HRESULT Ready_Components();

public:
    static CHub_SpellShop* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
//private:
//    class CUI_Spell_Shop* m_pUI_SpellShop = { nullptr };
private: //옵저버 관련
    vector<CObserver*> m_pObservers{};
public:
    void Add_Observer(CObserver* pObserver)
    {
        m_pObservers.push_back(pObserver);
    }
    virtual void Notify(void* pArg, const wstring& type)
    {
        for (auto& obs : m_pObservers)
            obs->OnNotify(&pArg, type);
    }
  
};
END
﻿#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "Hub_Portal.h"

BEGIN(Engine)
class CPickingSys;
END

BEGIN(Client)

class CLevel_Hub final : public CLevel
{
private:
	CLevel_Hub(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLevel_Hub() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_UI();
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const _wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Weapon();
	HRESULT Ready_Layer_Weapon_Icon();
	HRESULT Ready_Layer_Shop_UI();

private:
	CPickingSys* m_pPickingSys = { nullptr };
	CHub_Portal* m_pPortal{ nullptr };

public:
	static CLevel_Hub* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END
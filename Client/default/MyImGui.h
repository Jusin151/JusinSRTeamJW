#pragma once

#include "Base.h"

class CMyImGui final : public CBase
{
private:
	CMyImGui(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CMyImGui() = default;
public:
	HRESULT Initialize(LPDIRECT3DDEVICE9 pGraphic_Device);
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	LPDIRECT3DDEVICE9		m_pGraphic_Device = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

public:
	static CMyImGui* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	void Free();
};

#pragma once

#include "Base.h"

BEGIN(Engine)

class CMyImGui final : public CBase
{
private:
	CMyImGui(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CMyImGui() = default;
public:
	HRESULT Initialize(HWND hWnd, LPDIRECT3DDEVICE9 pGraphic_Device);
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	LPDIRECT3DDEVICE9		m_pGraphic_Device = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	bool show_demo_window = true;
	bool show_another_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

public:
	static CMyImGui* Create(HWND hWnd, LPDIRECT3DDEVICE9 pGraphic_Device);
	void Free();
};

END
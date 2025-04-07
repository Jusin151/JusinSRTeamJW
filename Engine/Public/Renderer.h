#pragma once

#include "Base.h"

/* 1. 오브젝트 매니져 하나만으로 그려야하는 순서, 모아놔야하는 레벨, 객쳊집합 고려하기가 어렵다. */
/* 2. 화면에 그려져야할 객체들을 그리는 순서대로 모아놓는다. */

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP
	{
		RG_PRIORITY,
		RG_NONBLEND,
		RG_BLEND,
		RG_VIEWMODEL,
		RG_UI_BACKGROUND,  //  UI 배경
		RG_UI_BUTTON,      //  UI 버튼
		RG_UI,             // 원래 UI
		RG_IMGUI,
		RG_END
	};

private:
	CRenderer(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CRenderer() = default;

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Set_Lights(class CShader* pShader);
	HRESULT Add_Light(class CLight* pLight);
	_uint	Get_NumOfLights() { return (_uint)m_Lights.size(); }
	HRESULT Draw();

private:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = { nullptr };

private:
	list<class CGameObject*>	m_RenderObjects[RG_END];
	list<class CLight*>			m_Lights;

private:
	HRESULT Enable_Lights();
	HRESULT Disable_Lights();
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_ViewModel();
	HRESULT Render_UI_Background(); 
	HRESULT Render_UI_Button();
	HRESULT Render_UI();

public:
	static CRenderer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END
#pragma once

#include "Base.h"

/* 1. 오브젝트 매니져 하나만으로 그려야하는 순서, 모아놔야하는 레벨, 객쳊집합 고려하기가 어렵다. */
/* 2. 화면에 그려져야할 객체들을 그리는 순서대로 모아놓는다. */

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP { RG_PRIORITY, RG_NONBLEND, RG_BLEND, RG_UI, RG_END };
private:
	CRenderer(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CRenderer() = default;

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Draw();

private:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = { nullptr };

private:
	list<class CGameObject*>	m_RenderObjects[RG_END];

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_UI();

public:
	static CRenderer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual void Free() override;
};

END
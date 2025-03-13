#include "UIObject.h"

CUIObject::CUIObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject { pGraphic_Device }
{
}

CUIObject::CUIObject(const CUIObject& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CUIObject::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CUIObject::Initialize(void* pArg)
{
	D3DXMatrixIdentity(&m_ViewMatrix);

	D3DVIEWPORT9            ViewportDesc{};
	m_pGraphic_Device->GetViewport(&ViewportDesc);

	/* w, h, n, f : ºäº¼·ýÀ» ¼³Á¤ÇÑ´Ù */
	D3DXMatrixOrthoLH(&m_ProjMatrix, ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f);

	return S_OK;
}

void CUIObject::Priority_Update(_float fTimeDelta)
{
}

void CUIObject::Update(_float fTimeDelta)
{
}

void CUIObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIObject::Render()
{
	return S_OK;
}
void CUIObject::Begin()
{
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &m_OldViewMatrix);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &m_OldProjMatrix);

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);
}

void CUIObject::End()
{
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_OldViewMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_OldProjMatrix);
}

void CUIObject::Free()
{
	__super::Free();
}

#include "Cursor.h"

CCursor::CCursor(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CUI_Base(pGraphic_Device)
{
}

CCursor::CCursor(const CCursor& rhs)
	: CUI_Base(rhs)
{
}

HRESULT CCursor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCursor::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CCursor::Priority_Update(_float fTimeDelta)
{
}

void CCursor::Update(_float fTimeDelta)
{
}

void CCursor::Late_Update(_float fTimeDelta)
{
}

HRESULT CCursor::Render()
{
	return E_NOTIMPL;
}

HRESULT CCursor::Ready_Components()
{
	return E_NOTIMPL;
}

CCursor* CCursor::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	return nullptr;
}

CGameObject* CCursor::Clone(void* pArg)
{
	return nullptr;
}

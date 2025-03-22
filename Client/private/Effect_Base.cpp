#include "Effect_Base.h"

CEffect_Base::CEffect_Base(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CEffect_Base::CEffect_Base(const CEffect_Base& Prototype)
	: CGameObject(Prototype),
	m_pTextureCom{ Prototype.m_pTextureCom },
	m_pTransformCom{ Prototype.m_pTransformCom },
	m_pVIBufferCom{ Prototype.m_pVIBufferCom }
{
}

HRESULT CEffect_Base::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CEffect_Base::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CEffect_Base::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Base::Update(_float fTimeDelta)
{
	m_fCurrentTime += fTimeDelta;

	if (m_fCurrentTime >= m_fLifeTime)
		m_bDead = true;
}


void CEffect_Base::Late_Update(_float fTimeDelta)
{
}

HRESULT CEffect_Base::Render()
{
	return E_NOTIMPL;
}

HRESULT CEffect_Base::Ready_Components()
{
	return E_NOTIMPL;
}

void CEffect_Base::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

}


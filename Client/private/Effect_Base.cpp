#include "Effect_Base.h"

CEffect_Base::CEffect_Base(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CEffect_Base::CEffect_Base(const CEffect_Base& Prototype)
	: CGameObject(Prototype),
	m_pTextureCom{ Prototype.m_pTextureCom },
	m_pTransformCom{ Prototype.m_pTransformCom },
	m_pVIBufferCom{ Prototype.m_pVIBufferCom },
	m_pParticleCom{ Prototype.m_pParticleCom }
{
}

HRESULT CEffect_Base::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Base::Initialize(void* pArg)
{
	if (pArg)
		m_Weapon_Effect_INFO = *reinterpret_cast<EFFECT_DESC*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(
		CTransform::STATE_POSITION,
		m_Weapon_Effect_INFO.vPos
	);

	if(m_pParticleCom) 
		m_pParticleCom->Set_Origin(m_Weapon_Effect_INFO.vPos);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, m_Weapon_Effect_INFO.vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, m_Weapon_Effect_INFO.vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_Weapon_Effect_INFO.vLook);

	m_pTransformCom->Set_Scale(
		m_Weapon_Effect_INFO.vScale.x,
		m_Weapon_Effect_INFO.vScale.y,
		m_Weapon_Effect_INFO.vScale.z);
	return S_OK;
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
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMaterialCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pParticleCom);
	Safe_Release(m_pTransformCom);
}


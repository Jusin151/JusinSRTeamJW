#include "Blood_Effect.h"

CBlood_Effect::CBlood_Effect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect_Base { pGraphic_Device }
{
}

CBlood_Effect::CBlood_Effect(const CBlood_Effect& Prototype)
	: CEffect_Base { Prototype }
	, m_pTarget { Prototype.m_pTarget }
{
}

HRESULT CBlood_Effect::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CBlood_Effect::Initialize(void* pArg)
{
	HIT_DESC desc = *reinterpret_cast<HIT_DESC*>(pArg);
	m_eHitType = (HitType)desc.type;
	return S_OK;
}

HRESULT CBlood_Effect::Ready_Components()
{
	switch (m_eHitType)
	{
	case HT_A:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Blood_Effect1"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;
	case HT_B:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Blood_Effect2"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBufferm"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	/*if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Firework"),
		TEXT("Com_Particle"), reinterpret_cast<CComponent**>(&m_pParticleCom), &FireworkDesc)))
		return E_FAIL;*/
	return S_OK;
}

void CBlood_Effect::Priority_Update(_float fTimeDelta)
{
}

void CBlood_Effect::Update(_float fTimeDelta)
{
}

void CBlood_Effect::Late_Update(_float fTimeDelta)
{
}

HRESULT CBlood_Effect::Pre_Render()
{
	return E_NOTIMPL;
}

HRESULT CBlood_Effect::Render()
{
	return E_NOTIMPL;
}

HRESULT CBlood_Effect::Post_Render()
{
	return E_NOTIMPL;
}

CBlood_Effect* CBlood_Effect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBlood_Effect* pInstance = new CBlood_Effect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed: CBlood_Effect");

		Safe_Release(pInstance);
	}

	return pInstance;
}

CBlood_Effect* CBlood_Effect::Clone(void* pArg)
{
	CBlood_Effect* pInstance = new CBlood_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{

		MSG_BOX("Clone Failed: CBlood_Effect");

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlood_Effect::Free()
{
}

#include "HellBoss_SkillBase.h"

CHellBoss_SkillBase::CHellBoss_SkillBase(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CEffect_Base{pGraphic_Device}
{
}

CHellBoss_SkillBase::CHellBoss_SkillBase(const CHellBoss_SkillBase& Prototype)
	:CEffect_Base(Prototype)
{
}

HRESULT CHellBoss_SkillBase::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHellBoss_SkillBase::Initialize(void* pArg)
{
	return S_OK;
}

void CHellBoss_SkillBase::Priority_Update(_float fTimeDelta)
{
}

void CHellBoss_SkillBase::Update(_float fTimeDelta)
{
}

void CHellBoss_SkillBase::Process_Input()
{
}

void CHellBoss_SkillBase::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CHellBoss_SkillBase::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CHellBoss_SkillBase::SetUp_RenderState()
{
	return S_OK;
}

HRESULT CHellBoss_SkillBase::Release_RenderState()
{
	return S_OK;
}

HRESULT CHellBoss_SkillBase::Ready_Components()
{




	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;



	return S_OK;
}

CHellBoss_SkillBase* CHellBoss_SkillBase::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHellBoss_SkillBase* pInstance = new CHellBoss_SkillBase(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHellBoss_SkillBase");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHellBoss_SkillBase::Clone(void* pArg)
{
	CHellBoss_SkillBase* pInstance = new CHellBoss_SkillBase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CHellBoss_SkillBase");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellBoss_SkillBase::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);


}


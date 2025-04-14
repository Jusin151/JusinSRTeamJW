#include "Projectile_Base.h"
#include "GameObject.h"
#include "GameInstance.h"

CProjectile_Base::CProjectile_Base(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCollisionObject{ pGraphic_Device }
{
}

CProjectile_Base::CProjectile_Base(const CProjectile_Base& Prototype)
	: CCollisionObject	{ Prototype }
	, m_vPos			{ Prototype.m_vPos }
	, m_vDir			{ Prototype.m_vDir }
	, m_fSpeed			{ Prototype.m_fSpeed }
	, m_pColliderCom	{ Prototype.m_pColliderCom }
	, m_pTextureCom		{ Prototype.m_pTextureCom }
	, m_pTransformCom	{ Prototype.m_pTransformCom }
	, m_pVIBufferCom	{ Prototype.m_pVIBufferCom }
{

}

HRESULT CProjectile_Base::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile_Base::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	

	return S_OK;
}

void CProjectile_Base::Priority_Update(_float fTimeDelta)
{
}

void CProjectile_Base::Update(_float fTimeDelta)
{
}

void CProjectile_Base::Late_Update(_float fTimeDelta)
{
}

HRESULT CProjectile_Base::Render()
{
	return S_OK;
}


HRESULT CProjectile_Base::Ready_Components()
{

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* For.Com_Material */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BaseShader"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}


void CProjectile_Base::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMaterialCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
}

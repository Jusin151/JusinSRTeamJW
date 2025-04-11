#include "WarningZone.h"


CWarningZone::CWarningZone(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CWarningZone::CWarningZone(const CWarningZone& Prototype)
	: CGameObject(Prototype),
	m_pTextureCom{ Prototype.m_pTextureCom },
	m_pVIBufferCom{ Prototype.m_pVIBufferCom },
	m_pShaderCom{ Prototype.m_pShaderCom },
	m_pMaterialCom{ Prototype.m_pMaterialCom },
	m_bIsCloned(true),
	m_bCanRender(Prototype.m_bCanRender)
{
	m_pTransformCom = nullptr;
}

HRESULT CWarningZone::Initialize_Prototype()
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	return S_OK;
}

HRESULT CWarningZone::Initialize(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	m_pTransformCom->Rotation(_float3(1.f, 0.f, 0.f), D3DXToRadian(90.f));
	m_vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_vOriginScale = m_pTransformCom->Compute_Scaled();
	return S_OK;
}

void CWarningZone::Update(_float fTimeDelta)
{
}

void CWarningZone::Late_Update(_float fTimeDelta)
{
	if (m_bCanRender)
	{
	m_pGameInstance->Add_RenderGroup(CRenderer::RG_BLEND, this);
	}
}

HRESULT CWarningZone::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(0)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	    m_pShaderCom->Set_Fog(_float3(1.f, 0.f, 0.f), 0.f, 15.f);
	if (FAILED(m_pShaderCom->Bind_Transform()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Lights()))
		return E_FAIL;
	SetUp_RenderState();
	if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, 0)))
		return E_FAIL;
	m_pShaderCom->Begin(0);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	m_pShaderCom->End();
	Release_RenderState();

	return S_OK;
}

void CWarningZone::Reset()
{

}

void CWarningZone::Set_Position(const _float3& vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

HRESULT CWarningZone::Ready_Components()
{
	// 컴포넌트 초기화 코드
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Vignette"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
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

HRESULT CWarningZone::SetUp_RenderState()
{
	D3DXVECTOR2 vScaleFactor(1.f, 1.f);
	D3DXVECTOR2 vOffsetFactor(0.0f, 0.0f); // Y축 반전을 위한 오프셋 조정
	m_pShaderCom->Set_UVScaleFactor(&vScaleFactor);
	m_pShaderCom->Set_UVOffsetFactor(&vOffsetFactor);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 240);
	return S_OK;
}

HRESULT CWarningZone::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

CWarningZone* CWarningZone::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWarningZone* pInstance = new CWarningZone(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to create CWarningZone instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CGameObject* CWarningZone::Clone(void* pArg)
{
	CWarningZone* pClone = new CWarningZone(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("Failed to clone CWarningZone instance");
		Safe_Release(pClone);
	}
	return pClone;
}

void CWarningZone::Free()
{
	__super::Free();
	if (!m_bIsCloned)
	{
		Safe_Release(m_pTextureCom);
		Safe_Release(m_pVIBufferCom);
		Safe_Release(m_pMaterialCom);
		Safe_Release(m_pShaderCom);
	}
	
	Safe_Release(m_pTransformCom);
}

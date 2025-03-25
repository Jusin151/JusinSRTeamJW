#include "Effect.h"

#include "GameInstance.h"

CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CBlendObject{ pGraphic_Device }
{
}

CEffect::CEffect(const CEffect& Prototype)
	: CBlendObject{ Prototype }
{
}

HRESULT CEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_pGameInstance->Compute_Random(0.f, 10.f), 
			0.0f , 
			m_pGameInstance->Compute_Random(0.f, 10.f)));

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION,
	//	_float3(rand() % 10,
	//		0.0f,
	//		rand() % 10));

	
	return S_OK;
}

void CEffect::Priority_Update(_float fTimeDelta)
{
}

void CEffect::Update(_float fTimeDelta)
{
	m_fFrame += 90.f * fTimeDelta;

	if (m_fFrame >= 90.f)
		m_fFrame = 0.f;

	Compute_CameraDepth(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_float4x4       ViewMatrix{};

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);
	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);	

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *reinterpret_cast<_float3*>(&ViewMatrix.m[0][0]));
	m_pTransformCom->Set_State(CTransform::STATE_UP, *reinterpret_cast<_float3*>(&ViewMatrix.m[1][0]));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *reinterpret_cast<_float3*>(&ViewMatrix.m[2][0]));
}

void CEffect::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

HRESULT CEffect::Render()
{

	if (FAILED(m_pTextureCom->Bind_Resource(static_cast<_uint>(m_fFrame))))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	SetUp_RenderState();

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	Release_RenderState();

	return S_OK;
}

HRESULT CEffect::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 50);
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CEffect::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);	
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	// m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CEffect::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

CEffect* CEffect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect* pInstance = new CEffect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect::Clone(void* pArg)
{
	CEffect* pInstance = new CEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);



}

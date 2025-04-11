#include "Stains_Effect.h"
#include "Particles.h"

CStains_Effect::CStains_Effect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect_Base{ pGraphic_Device }
{
}

CStains_Effect::CStains_Effect(const CStains_Effect& Prototype)
	: CEffect_Base{ Prototype }
	, m_pTarget{ Prototype.m_pTarget }
{
}

HRESULT CStains_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStains_Effect::Initialize(void* pArg)
{
	_float3 tmpPos = {};
	HIT_DESC desc = *reinterpret_cast<HIT_DESC*>(pArg);
	m_eHitType = (HitType)desc.type;
	tmpPos = desc.vPos;
	desc.vPos.y = -0.45f;
	
	if (FAILED(__super::Initialize(&desc)))
		return E_FAIL;
	m_fAnimationSpeed = { 10.0f };
	m_iLastFrame = { m_pTextureCom->Get_NumTextures() };
	m_iCurrentFrame = { rand() % m_iLastFrame };

	return S_OK;
}

HRESULT CStains_Effect::Ready_Components()
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

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BaseShader"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Material */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	return S_OK;
}

void CStains_Effect::Priority_Update(_float fTimeDelta)
{
}

void CStains_Effect::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	m_fElapsedTime += fTimeDelta;

	if (m_fElapsedTime >= m_fAnimationSpeed)
	{
		m_bIsActive = false;
		m_bDead = true;
	}
	if(m_pParticleCom)
		m_pParticleCom->Update(fTimeDelta);
}

void CStains_Effect::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CStains_Effect::Pre_Render()
{
	D3DXVECTOR2 vScaleFactor(1.f, 1.f);
	D3DXVECTOR2 vOffsetFactor(0.0f, 0.0f); // Y축 반전을 위한 오프셋 조정
	m_pShaderCom->Set_UVScaleFactor(&vScaleFactor);
	m_pShaderCom->Set_UVOffsetFactor(&vOffsetFactor);
	/*m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);*/
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)
	return S_OK;
}

HRESULT CStains_Effect::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_Transform()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, m_iCurrentFrame)))
		return E_FAIL;
	Pre_Render();
	m_pShaderCom->Begin(1);
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	m_pShaderCom->End();
	Post_Render();
	
	return S_OK;
}

HRESULT CStains_Effect::Post_Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	return S_OK;
}

CStains_Effect* CStains_Effect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CStains_Effect* pInstance = new CStains_Effect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed: CStains_Effect");

		Safe_Release(pInstance);
	}

	return pInstance;
}

CStains_Effect* CStains_Effect::Clone(void* pArg)
{
	CStains_Effect* pInstance = new CStains_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{

		MSG_BOX("Clone Failed: CStains_Effect");

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStains_Effect::Free()
{
	__super::Free();
}

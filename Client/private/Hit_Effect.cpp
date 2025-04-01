#include "Hit_Effect.h"

CHit_Effect::CHit_Effect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect_Base { pGraphic_Device }
{
}

CHit_Effect::CHit_Effect(const CHit_Effect& Prototype)
	: CEffect_Base { Prototype }
	, m_pTarget { Prototype.m_pTarget }
{

}

HRESULT CHit_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHit_Effect::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		HIT_DESC desc = *reinterpret_cast<HIT_DESC*>(pArg);
		m_eHitType = (HitType)desc.type;
	}
	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fAnimationSpeed = { 0.2f };
	m_iLastFrame = { m_pTextureCom->Get_NumTextures() };
	m_iCurrentFrame = { 0 };
	return S_OK;
}

HRESULT CHit_Effect::Ready_Components()
{
	switch (m_eHitType)
	{
	case HT_A:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit_Effect1"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;
	case HT_B:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit_Effect2"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;
	case HT_C:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit_Effect3"),
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
		break;
	case HT_D:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit_Effect4"),
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

void CHit_Effect::Priority_Update(_float fTimeDelta)
{

}

void CHit_Effect::Update(_float fTimeDelta)
{
	m_fElapsedTime += fTimeDelta;

	if (m_fElapsedTime >= m_fAnimationSpeed)
	{
		m_fElapsedTime = 0.0f;

		if (m_iCurrentFrame < m_iLastFrame)
		{
			m_iCurrentFrame++;
		}
		else
		{
			//처음 루프이후 다음루프라면 이펙트를 끈다.
			//m_bDead = true;
			m_iCurrentFrame = 0;
		}
	}
}

void CHit_Effect::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CHit_Effect::Pre_Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)
	return S_OK;
}

HRESULT CHit_Effect::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	Pre_Render();

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	Post_Render();

	/*if (FAILED(m_pParticleCom->Render()))
		return E_FAIL;*/
	return S_OK;
}

HRESULT CHit_Effect::Post_Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

CHit_Effect* CHit_Effect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHit_Effect* pInstance = new CHit_Effect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{

		MSG_BOX("Create Failed: CHit_Effect");

		Safe_Release(pInstance);
	}

	return pInstance;
}

CHit_Effect* CHit_Effect::Clone(void* pArg)
{
	CHit_Effect* pInstance = new CHit_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{

		MSG_BOX("Clone Failed: CHit_Effect");

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHit_Effect::Free()
{
	__super::Free();
}

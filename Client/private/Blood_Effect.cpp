#include "Blood_Effect.h"
#include "Particles.h"

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
	return S_OK;
}

HRESULT CBlood_Effect::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		HIT_DESC desc = *reinterpret_cast<HIT_DESC*>(pArg);
		m_eHitType = (HitType)desc.type;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fAnimationSpeed = { 0.08f };
	m_iLastFrame = { m_pTextureCom->Get_NumTextures() };
	m_iCurrentFrame = { 0 };

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

	CBlood_Particle_System::BLOODDESC bloodDesc = {};
	bloodDesc.iNumParticles = { 10u };
	bloodDesc.Bound.m_vCenter = { 0.f, 0.f, 0.f };
	bloodDesc.Bound.m_fRadius = 0.1f;
	bloodDesc.strTexturePath = L"../../Resources/Textures/Particle/particle_gore_%d.png";
	bloodDesc.iNumTextures = 3;

	/* For.Com_BloodParticle */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Blood"),
		TEXT("Com_Particle"), reinterpret_cast<CComponent**>(&m_pParticleCom), &bloodDesc)))
		return E_FAIL;
	return S_OK;
}

void CBlood_Effect::Priority_Update(_float fTimeDelta)
{
}

void CBlood_Effect::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
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
			m_bDead = true;
			//m_iCurrentFrame = 0;
		}
	}
	m_pParticleCom->Update(fTimeDelta);
}

void CBlood_Effect::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_BLEND, this)))
		return;
}

HRESULT CBlood_Effect::Pre_Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)
	return S_OK;
}

HRESULT CBlood_Effect::Render()
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

	if (FAILED(m_pParticleCom->Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CBlood_Effect::Post_Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	return S_OK;
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
	__super::Free();
}

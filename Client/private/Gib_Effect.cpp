#include "Gib_Effect.h"
#include "Particles.h"

CGib_Effect::CGib_Effect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect_Base{ pGraphic_Device }
{
}

CGib_Effect::CGib_Effect(const CGib_Effect& Prototype)
	: CEffect_Base{ Prototype }
	, m_pTarget{ Prototype.m_pTarget }
{
}

HRESULT CGib_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGib_Effect::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		HIT_DESC desc = *reinterpret_cast<HIT_DESC*>(pArg);
		m_eMonsterType = (MonsterType)desc.type;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fLifeTime = 5.f;
	static_cast<CGib_Particle_System*>(m_pParticleCom)->Set_WorldMat(*m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

HRESULT CGib_Effect::Ready_Components()
{
	CGib_Particle_System::GIBDESC GibDesc = {};
	GibDesc.iNumParticles = { 10u };
	GibDesc.Bound.m_vCenter = { 0.f, 0.f, 0.f };
	GibDesc.Bounding_Box.m_vMin = { 0.f, -m_Weapon_Effect_INFO.vPos.y, 0.f};
	GibDesc.Bounding_Box.m_vMax = { 1.f, 1.f, 1.f};
	GibDesc.Bound.m_fRadius = 0.1f;
	GibDesc.strTexturePath = L"../../Resources/Textures/Particle/particle_gore_%d.png";
	GibDesc.iNumTextures = 3;

	switch (m_eMonsterType)
	{
	case Client::CGib_Effect::MT_NORMAL:
		GibDesc.strTexturePath = L"../../Resources/Textures/Effect/gib/gib_%d.png";
		GibDesc.iNumTextures = 3;
		break;
	case Client::CGib_Effect::MT_SNOWSPIDER:
		GibDesc.strTexturePath = L"../../Resources/Textures/Effect/gib/gib_snowspider_%d.png";
		GibDesc.iNumTextures = 3;
		break;
	case Client::CGib_Effect::MT_YETI:
		GibDesc.strTexturePath = L"../../Resources/Textures/Effect/gib/gib_yeti_%d.png";
		GibDesc.iNumTextures = 3;
		break;
	case Client::CGib_Effect::MT_GLACIER_HIT:
		GibDesc.strTexturePath = L"../../Resources/Textures/Effect/gib/gib_glacier_%d.png";
		GibDesc.iNumTextures = 5;
		break;
	case Client::CGib_Effect::MT_GLACIER_DEAD:
		GibDesc.strTexturePath = L"../../Resources/Textures/Effect/gib/shard_ice_%d.png";
		GibDesc.iNumTextures = 2;
		break;
	case Client::CGib_Effect::MT_END:
		break;
	default:
		break;
	}

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	/* For.Com_GibParticle */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Gib"),
		TEXT("Com_Particle"), reinterpret_cast<CComponent**>(&m_pParticleCom), &GibDesc)))
		return E_FAIL;

	return S_OK;
}

void CGib_Effect::Priority_Update(_float fTimeDelta)
{
}

void CGib_Effect::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	m_pParticleCom->Update(fTimeDelta);
}

void CGib_Effect::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CGib_Effect::Pre_Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)
	return S_OK;
}

HRESULT CGib_Effect::Render()
{
	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;
	Pre_Render();
	if (FAILED(m_pParticleCom->Render()))
		return E_FAIL;
	Post_Render();
	return S_OK;
}

HRESULT CGib_Effect::Post_Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	return S_OK;
}

CGib_Effect* CGib_Effect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGib_Effect* pInstance = new CGib_Effect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed: CGib_Effect");

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGib_Effect* CGib_Effect::Clone(void* pArg)
{
	CGib_Effect* pInstance = new CGib_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{

		MSG_BOX("Clone Failed: CGib_Effect");

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGib_Effect::Free()
{
	__super::Free();
}

#include "Magic_Effect.h"
#include "Particles.h"

CMagic_Effect::CMagic_Effect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect_Base{ pGraphic_Device }
{
}

CMagic_Effect::CMagic_Effect(const CMagic_Effect& Prototype)
	: CEffect_Base{ Prototype }
	, m_pTarget{ Prototype.m_pTarget }
{
}

HRESULT CMagic_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMagic_Effect::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		HIT_DESC desc = *reinterpret_cast<HIT_DESC*>(pArg);
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fLifeTime = 5.f;
	return S_OK;
}

HRESULT CMagic_Effect::Ready_Components()
{
	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	CMagic_Particle_System::MAGICDESC MagicDesc = {};
	MagicDesc.iNumParticles = { 10u };
	MagicDesc.vOrigin = m_Weapon_Effect_INFO.vPos;
	MagicDesc.Bound.m_vCenter = { 0.f, 0.f, 0.f };
	MagicDesc.Bound.m_fRadius = 0.01f;
	MagicDesc.strTexturePath = L"../../Resources/Textures/Particle/sprite_blood_particle.png";
	MagicDesc.fSize = 0.1f;
	MagicDesc.iNumTextures = 1;

	/* For.Com_MagicParticle */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Magic"),
		TEXT("Com_Particle"), reinterpret_cast<CComponent**>(&m_pParticleCom), &MagicDesc)))
		return E_FAIL;

	return S_OK;
}

void CMagic_Effect::Priority_Update(_float fTimeDelta)
{
}

void CMagic_Effect::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	m_pParticleCom->Update(fTimeDelta);
}

void CMagic_Effect::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_BLEND, this)))
		return;
}

HRESULT CMagic_Effect::Pre_Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)
	return S_OK;
}

HRESULT CMagic_Effect::Render()
{
	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;
	Pre_Render();
	if (FAILED(m_pParticleCom->Render()))
		return E_FAIL;
	Post_Render();
	return S_OK;
}

HRESULT CMagic_Effect::Post_Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	return S_OK;
}

CMagic_Effect* CMagic_Effect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMagic_Effect* pInstance = new CMagic_Effect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed: CMagic_Effect");

		Safe_Release(pInstance);
	}

	return pInstance;
}

CMagic_Effect* CMagic_Effect::Clone(void* pArg)
{
	CMagic_Effect* pInstance = new CMagic_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{

		MSG_BOX("Clone Failed: CMagic_Effect");

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMagic_Effect::Free()
{
	__super::Free();
}

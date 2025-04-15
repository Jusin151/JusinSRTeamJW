#include "Fire_Effect.h"
#include "Particles.h"

CFire_Effect::CFire_Effect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect_Base{ pGraphic_Device }
{
}

CFire_Effect::CFire_Effect(const CFire_Effect& Prototype)
	: CEffect_Base{ Prototype }
	, m_pTarget{ Prototype.m_pTarget }
{
}

HRESULT CFire_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFire_Effect::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		HIT_DESC desc = *reinterpret_cast<HIT_DESC*>(pArg);
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fLifeTime = 5.f;
	static_cast<CFire_Particle_System*>(m_pParticleCom)->Set_WorldMat(*m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

HRESULT CFire_Effect::Ready_Components()
{
	CFire_Particle_System::FIREDESC FireDesc = {};
	FireDesc.iNumParticles = { 10u };
	FireDesc.Bound.m_vCenter = { 0.f, 0.f, 0.f };
	FireDesc.Bounding_Box.m_vMin = { 0.f, m_Weapon_Effect_INFO.vPos.y, 0.f };
	FireDesc.Bounding_Box.m_vMax = { 1.f, 1.f, 1.f };
	FireDesc.Bound.m_fRadius = 0.1f;
	FireDesc.strTexturePath = L"../../Resources/Textures/Effect/Fire/fire_ground_000%d.png";
	FireDesc.iNumTextures = 7;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	/* For.Com_FireParticle */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Fire"),
		TEXT("Com_Particle"), reinterpret_cast<CComponent**>(&m_pParticleCom), &FireDesc)))
		return E_FAIL;

	return S_OK;
}

void CFire_Effect::Priority_Update(_float fTimeDelta)
{
}

void CFire_Effect::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	m_pParticleCom->Update(fTimeDelta);
}

void CFire_Effect::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_BLEND, this)))
		return;
}

HRESULT CFire_Effect::Pre_Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)
	return S_OK;
}

HRESULT CFire_Effect::Render()
{
	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;
	Pre_Render();
	if (FAILED(m_pParticleCom->Render()))
		return E_FAIL;
	Post_Render();
	return S_OK;
}

HRESULT CFire_Effect::Post_Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	return S_OK;
}

CFire_Effect* CFire_Effect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFire_Effect* pInstance = new CFire_Effect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed: CFire_Effect");

		Safe_Release(pInstance);
	}

	return pInstance;
}

CFire_Effect* CFire_Effect::Clone(void* pArg)
{
	CFire_Effect* pInstance = new CFire_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{

		MSG_BOX("Clone Failed: CFire_Effect");

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFire_Effect::Free()
{
	__super::Free();
}

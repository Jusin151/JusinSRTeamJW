#include "BulletShell_Effect.h"
#include "Particles.h"

CBulletShell_Effect::CBulletShell_Effect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect_Base{ pGraphic_Device }
{
}

CBulletShell_Effect::CBulletShell_Effect(const CBulletShell_Effect& Prototype)
	: CEffect_Base{ Prototype }
	, m_pTarget{ Prototype.m_pTarget }
{
}

HRESULT CBulletShell_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBulletShell_Effect::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		BULLETSHELLDESC desc = *reinterpret_cast<BULLETSHELLDESC*>(pArg);
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fLifeTime = 5.f;
	static_cast<CBulletShell_Particle_System*>(m_pParticleCom)->Set_WorldMat(*m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

HRESULT CBulletShell_Effect::Ready_Components()
{
	CBulletShell_Particle_System::BULLETSHELLDESC BulletShellDesc = {};
	BulletShellDesc.iNumParticles = { 1u };
	BulletShellDesc.Bound.m_vCenter = { 0.f, 0.f, 0.f };
	BulletShellDesc.Bounding_Box.m_vMin = { 0.f, -m_Weapon_Effect_INFO.vPos.y, 0.f };
	BulletShellDesc.Bounding_Box.m_vMax = { 1.f, 1.f, 1.f };
	BulletShellDesc.Bound.m_fRadius = 0.1f;
	BulletShellDesc.strTexturePath = L"../../Resources/Textures/Particle/minigun_bullet_casing.png";
	BulletShellDesc.iNumTextures = 1;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	/* For.Com_BulletShellParticle */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_BulletShell"),
		TEXT("Com_Particle"), reinterpret_cast<CComponent**>(&m_pParticleCom), &BulletShellDesc)))
		return E_FAIL;

	return S_OK;
}

void CBulletShell_Effect::Priority_Update(_float fTimeDelta)
{
}

void CBulletShell_Effect::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	m_pParticleCom->Update(fTimeDelta);
}

void CBulletShell_Effect::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CBulletShell_Effect::Pre_Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)
	return S_OK;
}

HRESULT CBulletShell_Effect::Render()
{
	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;
	Pre_Render();
	if (FAILED(m_pParticleCom->Render()))
		return E_FAIL;
	Post_Render();
	return S_OK;
}

HRESULT CBulletShell_Effect::Post_Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	return S_OK;
}

CBulletShell_Effect* CBulletShell_Effect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBulletShell_Effect* pInstance = new CBulletShell_Effect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed: CBulletShell_Effect");

		Safe_Release(pInstance);
	}

	return pInstance;
}

CBulletShell_Effect* CBulletShell_Effect::Clone(void* pArg)
{
	CBulletShell_Effect* pInstance = new CBulletShell_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{

		MSG_BOX("Clone Failed: CBulletShell_Effect");

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBulletShell_Effect::Free()
{
	__super::Free();
}

#include "Weapon_Effect.h"
#include "Player.h"
#include "Firework_Particle_System.h"

CWeapon_Effect::CWeapon_Effect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CEffect_Base{ pGraphic_Device }
{
}

CWeapon_Effect::CWeapon_Effect(const CWeapon_Effect& Prototype)
	: CEffect_Base(Prototype)
{
}
 
HRESULT CWeapon_Effect::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CWeapon_Effect::Initialize(void* pArg)
{
	if (pArg)
		m_Weapon_Effect_INFO = *reinterpret_cast<EFFECT_DESC*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(
		CTransform::STATE_POSITION,
		m_Weapon_Effect_INFO.vPos
	);

	m_pParticleCom->Set_Origin(m_Weapon_Effect_INFO.vPos);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, m_Weapon_Effect_INFO.vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, m_Weapon_Effect_INFO.vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_Weapon_Effect_INFO.vLook);

	m_pTransformCom->Set_Scale(
		m_Weapon_Effect_INFO.vScale.x,
		m_Weapon_Effect_INFO.vScale.y,
		m_Weapon_Effect_INFO.vScale.z);


	return S_OK;
}


void CWeapon_Effect::Priority_Update(_float fTimeDelta)
{


}

void CWeapon_Effect::Update(_float fTimeDelta)
{
	
	m_pTransformCom->Set_Scale(
		m_Weapon_Effect_INFO.vScale.x,
		m_Weapon_Effect_INFO.vScale.y,
		m_Weapon_Effect_INFO.vScale.z
	);

	m_pParticleCom->Update(fTimeDelta);
}


void CWeapon_Effect::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
		return;	

}
HRESULT CWeapon_Effect::SetUp_RenderState()
{
	// 일단 추가해보기
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)

	return S_OK;
}
HRESULT CWeapon_Effect::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CWeapon_Effect::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(0)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	SetUp_RenderState();

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	Release_RenderState();

	if (FAILED(m_pParticleCom->Render()))
		return E_FAIL;
	


	return S_OK;
}


HRESULT CWeapon_Effect::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Weapon_Effect"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBufferm"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	CFirework_Particle_System::FIREWORKDESC     FireworkDesc{};
	FireworkDesc.fSize = 0.2f;
	FireworkDesc.Bounding_Box.m_vMin = { -2, -2, -2 };
	FireworkDesc.Bounding_Box.m_vMax = { 2, 2, 2 };
	FireworkDesc.vOrigin = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	FireworkDesc.iNumParticles = 100;
	FireworkDesc.strTexturePath = L"../../Resources/Textures/Particle/sprite_blood_particle.png";

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Firework"),
		TEXT("Com_Particle"), reinterpret_cast<CComponent**>(&m_pParticleCom), &FireworkDesc)))
		return E_FAIL;

	return S_OK;
}

CWeapon_Effect* CWeapon_Effect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWeapon_Effect* pInstance = new CWeapon_Effect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{

		MSG_BOX("웨폰이펙트 UI 원본 생성 실패 ");

		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CWeapon_Effect::Clone(void* pArg)
{
	CWeapon_Effect* pInstace = new CWeapon_Effect(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("웨폰이펙트 UI 복제 실패");

		Safe_Release(pInstace);
	}

	return pInstace;
}

void CWeapon_Effect::Free()
{
	__super::Free();

}

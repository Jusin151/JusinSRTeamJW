#include "HellBoss_Bullet.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"
#include "Particles.h"

CHellBoss_Bullet::CHellBoss_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CBullet_Base(pGraphic_Device)
{
}

CHellBoss_Bullet::CHellBoss_Bullet(const CHellBoss_Bullet& Prototype)
	:CBullet_Base( Prototype )
{
}

HRESULT CHellBoss_Bullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHellBoss_Bullet::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_HellBoss_Transform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Instance()->Get_Component(LEVEL_HONG, TEXT("Layer_HellBoss"), TEXT("Com_Transform")));
	if (m_HellBoss_Transform == nullptr)
		return E_FAIL; 

	m_fHellBoss_RIght = m_HellBoss_Transform->Get_State(CTransform::STATE_RIGHT);
	m_fHellBoss_Up = m_HellBoss_Transform->Get_State(CTransform::STATE_UP);
	m_fHellBoss_Look = m_HellBoss_Transform->Get_State(CTransform::STATE_LOOK);
	m_fHellBoss_Pos = m_HellBoss_Transform->Get_State(CTransform::STATE_POSITION);



	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, m_fHellBoss_RIght);
	m_pTransformCom->Set_State(CTransform::STATE_UP, m_fHellBoss_Up);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_fHellBoss_Look);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_fHellBoss_Pos);
	m_pTransformCom->Set_Scale(0.6f, 0.6f, 0.6f);


	m_fSpeed = 0.1f;
	m_vDir = m_HellBoss_Transform->Get_State(CTransform::STATE_LOOK);
	return S_OK;
}

void CHellBoss_Bullet::Priority_Update(_float fTimeDelta)
{
	m_fLifeTime += fTimeDelta;

	if (m_fLifeTime >= 2.f)
	{
		m_bIsActive = false;
		m_fLifeTime = 0.f;
	}
}

void CHellBoss_Bullet::Update(_float fTimeDelta)
{
	
	m_pTransformCom->Go(m_vDir, fTimeDelta * m_fSpeed);
	m_pParticleTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_float3 a = m_HellBoss_Transform->Get_State(CTransform::STATE_POSITION);
	_float3 b = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 dir = b - a;
	D3DXVec3Normalize(&dir, &dir);
	dynamic_cast<CProjectile_Particle_System*>(m_pParticleCom)->Set_Dir(m_vDir);
	m_pParticleCom->Update(fTimeDelta);
	m_fElapsedTime += fTimeDelta;
	if (m_fElapsedTime >= 0.02f) 
	{
		m_fElapsedTime = 0.0f;
		if (m_iCurrentFrame < 6) 
		{
			m_iCurrentFrame = (m_iCurrentFrame + 1) % 7; 
		}
		else
		{
			m_iCurrentFrame = 6;
		}
	}
}



void CHellBoss_Bullet::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}


HRESULT CHellBoss_Bullet::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	SetUp_RenderState();

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	if (FAILED(m_pParticleTransformCom->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_pParticleCom->Render()))
		return E_FAIL;

	Release_RenderState();

	return S_OK;
}

HRESULT CHellBoss_Bullet::On_Collision(CCollisionObject* other)
{
	__super::On_Collision(other);

	

	return S_OK;
}


void CHellBoss_Bullet::Attack_Melee()
{
	m_pAttackCollider->Update_Collider(TEXT("Com_Transform"), m_pAttackCollider->Get_Scale());

	m_pGameInstance->Add_Collider(CG_MONSTER_PROJECTILE_CUBE, m_pAttackCollider);
}

void CHellBoss_Bullet::Reset()
{
	m_fElapsedTime = 0.f;
	m_iCurrentFrame = 0;

	if (!m_HellBoss_Transform) return;

	m_fHellBoss_RIght = m_HellBoss_Transform->Get_State(CTransform::STATE_RIGHT);
	m_fHellBoss_Up = m_HellBoss_Transform->Get_State(CTransform::STATE_UP);
	m_fHellBoss_Look = m_HellBoss_Transform->Get_State(CTransform::STATE_LOOK);
	m_fHellBoss_Pos = m_HellBoss_Transform->Get_State(CTransform::STATE_POSITION);


	/*Player_Pos += Player_Look * 0.8f;
	Player_Pos += Player_RIght * 0.1f;
	Player_Pos.y -= 0.5f;*/

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, m_fHellBoss_RIght);
	m_pTransformCom->Set_State(CTransform::STATE_UP, m_fHellBoss_Up);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_fHellBoss_Look);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_fHellBoss_Pos);
	m_pTransformCom->Set_Scale(0.6f, 0.6f, 0.6f);
	m_vDir = m_HellBoss_Transform->Get_State(CTransform::STATE_LOOK);

}

HRESULT CHellBoss_Bullet::SetUp_RenderState()
{
	// 일단 추가해보기

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)

	return S_OK;
}

HRESULT CHellBoss_Bullet::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CHellBoss_Bullet::Ready_Components()
{

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBufferm"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	_float3 fPos = { 10.f, 0.5f, 10.f };

	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 1.f, 1.f, 1.f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.5f, 0.f };


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackCollider), &ColliderDesc)))
		return E_FAIL;


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Weapon_Bullet"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	CProjectile_Particle_System::TRAILDESC     trailDesc{};
	trailDesc.fDistance = 30.f;
	trailDesc.fWidth = 1.f;
	trailDesc.iNumParticles = 1;
	trailDesc.strTexturePath = L"../../Resources/Textures/Particle/sprite_blood_particle.png";
	trailDesc.iNumTextures = 1;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Projectile"),
		TEXT("Com_Particle"), reinterpret_cast<CComponent**>(&m_pParticleCom), &trailDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_ParticleTransform"), reinterpret_cast<CComponent**>(&m_pParticleTransformCom))))
		return E_FAIL;

	return S_OK;
}

CHellBoss_Bullet* CHellBoss_Bullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHellBoss_Bullet* pInstance = new CHellBoss_Bullet(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHellBoss_Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHellBoss_Bullet::Clone(void* pArg)
{
	CHellBoss_Bullet* pInstance = new CHellBoss_Bullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CHellBoss_Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellBoss_Bullet::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pAttackCollider);
	Safe_Release(m_pParticleCom);
	Safe_Release(m_pParticleTransformCom);
}

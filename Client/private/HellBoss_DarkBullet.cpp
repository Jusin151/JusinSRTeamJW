#include "HellBoss_DarkBullet.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"
#include "Particles.h"
#include "Camera_FirstPerson.h" 

CHellBoss_DarkBullet::CHellBoss_DarkBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CBullet_Base(pGraphic_Device)
{
}

CHellBoss_DarkBullet::CHellBoss_DarkBullet(const CHellBoss_DarkBullet& Prototype)
	:CBullet_Base( Prototype )
{
}

HRESULT CHellBoss_DarkBullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHellBoss_DarkBullet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_fSpeed = 5.f;
	m_iAp = 1;
	m_eType = CG_MONSTER_PROJECTILE_SPHERE;
	//Find_Last_Object(LEVEL_HONG, L"Layer_HellBoss_Skill_DarkHole")

	m_pDarkHole_Transform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Instance()
		->Find_Last_Object(LEVEL_HONG, L"Layer_HellBoss_Skill_DarkHole")
		->Get_Component(TEXT("Com_Transform")));


	
	m_Player_Transform; //여기 플레이어 좌표 받아놨음
	m_vDir = m_pDarkHole_Transform->Get_State(CTransform::STATE_LOOK);
	return S_OK;
}

void CHellBoss_DarkBullet::Priority_Update(_float fTimeDelta)
{

	m_fLifeTime += fTimeDelta;

	if (m_fLifeTime >= 5.f)
	{
		m_bIsActive = false;
		m_fLifeTime = 0.f;
	}
}

void CHellBoss_DarkBullet::Update(_float fTimeDelta)
{
	
	m_pTransformCom->Go(m_vDir, fTimeDelta * m_fSpeed);
	m_pParticleTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	dynamic_cast<CTrail_Particle_System*>(m_pParticleCom)->Set_Dir(m_vDir);
	m_pParticleCom->Update(fTimeDelta);
	m_fElapsedTime += fTimeDelta;
	if (m_fElapsedTime >= 0.02f) 
	{
		m_fElapsedTime = 0.0f;
		if (m_iCurrentFrame < 6) 
		{
			m_iCurrentFrame = (m_iCurrentFrame + 1) % 7; // 팀장님이 알려준
		}
		else
		{
			m_iCurrentFrame = 6;
		}
	}

	

	//_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//if (fabs(vPos.y) <= 0.1f) 
	//{

	//	CGameObject* pCamera = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Camera"));
	//	if (pCamera)
	//	{
	//		CCamera_FirstPerson* pFirstPersonCam = dynamic_cast<CCamera_FirstPerson*>(pCamera);
	//		if (pFirstPersonCam)
	//		{
	//			pFirstPersonCam->TriggerShake_HellBoss(0.4f, 0.5f);
	//		}
	//	}
	//}

	m_pAttackCollider->Update_Collider(TEXT("Com_Transform"), m_pAttackCollider->Get_Scale());

	m_pGameInstance->Add_Collider(CG_MONSTER_PROJECTILE_SPHERE, m_pAttackCollider);

}



void CHellBoss_DarkBullet::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}


HRESULT CHellBoss_DarkBullet::Render()
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

HRESULT CHellBoss_DarkBullet::On_Collision(CCollisionObject* other)
{
	if (nullptr == m_pColliderCom && nullptr == m_pAttackCollider)
		return E_FAIL;

	if (nullptr == other)
		return S_OK;

	// 안바뀌면 충돌 안일어남
	if (other->Get_Type() == CG_END)
		return S_OK;

	switch (other->Get_Type())
	{
	case CG_PLAYER:
		Take_Damage(other);
		m_bIsActive = false;
		break;

	case CG_WEAPON:



		break;
	default:
		break;
	}



	return S_OK;
}



void CHellBoss_DarkBullet::Attack_Melee()
{
	
}

void CHellBoss_DarkBullet::Reset()
{
	m_pDarkHole_Transform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Instance()
		->Find_Last_Object(LEVEL_HONG, L"Layer_HellBoss_Skill_DarkHole")
		->Get_Component(TEXT("Com_Transform")));

	m_fElapsedTime = 0.f;
	m_iCurrentFrame = 0;

	if (!m_pDarkHole_Transform) return;

	DarkHole_RIght = m_pDarkHole_Transform->Get_State(CTransform::STATE_RIGHT);
	DarkHole_Up = m_pDarkHole_Transform->Get_State(CTransform::STATE_UP);
	DarkHole_Look = m_pDarkHole_Transform->Get_State(CTransform::STATE_LOOK);
	DarkHole_Pos = m_pDarkHole_Transform->Get_State(CTransform::STATE_POSITION);


	/*DarkHole_Pos += DarkHole_Look * 0.8f;
	DarkHole_Pos += DarkHole_RIght * 0.1f;
	DarkHole_Pos.y -= 0.5f;*/

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, DarkHole_RIght);
	m_pTransformCom->Set_State(CTransform::STATE_UP, DarkHole_Up);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, DarkHole_Look);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, DarkHole_Pos);
	m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);


	if (m_Player_Transform)
	{
		_float3 vPlayerPos = m_Player_Transform->Get_State(CTransform::STATE_POSITION);
		_float3 vBulletPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		// 방향 벡터 계산
		m_vDir = (vPlayerPos - vBulletPos);

		m_vDir.Normalize();
	}
	else
	{
		// fallback: 기존 DarkHole 방향 유지
		m_vDir = m_pDarkHole_Transform->Get_State(CTransform::STATE_LOOK);
	}

	m_eType = CG_MONSTER_PROJECTILE_SPHERE;
	m_iAp = 1;

}



HRESULT CHellBoss_DarkBullet::SetUp_RenderState()
{
	// 일단 추가해보기

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)

	return S_OK;
}

HRESULT CHellBoss_DarkBullet::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CHellBoss_DarkBullet::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Weapon_Bullet"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	CTrail_Particle_System::TRAILDESC     trailDesc{};
	trailDesc.fDistance = 20000.f;
	trailDesc.fWidth = 1.f;
	trailDesc.iNumParticles = 1;
	trailDesc.strTexturePath = L"../../Resources/Textures/Particle/sprite_blood_particle.png";
	trailDesc.iNumTextures = 1;

	    //1번은 꼬리 길이
		//2번은 꼬리 넓이
		//3번은 1개 고정
		//4번은 꼬리로 쓸 텍스쳐
		//5번은 텍스쳐 애니메이션있으면 쓸 텍스쳐 갯수

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Trail"),
		TEXT("Com_Particle"), reinterpret_cast<CComponent**>(&m_pParticleCom), &trailDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_ParticleTransform"), reinterpret_cast<CComponent**>(&m_pParticleTransformCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 1.f, 1.f, 1.f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.5f, 0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackCollider), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CHellBoss_DarkBullet* CHellBoss_DarkBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHellBoss_DarkBullet* pInstance = new CHellBoss_DarkBullet(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHellBoss_DarkBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHellBoss_DarkBullet::Clone(void* pArg)
{
	CHellBoss_DarkBullet* pInstance = new CHellBoss_DarkBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CHellBoss_DarkBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellBoss_DarkBullet::Free()
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

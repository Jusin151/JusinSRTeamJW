#include "HellBoss_Bullet.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"
#include "Particles.h"

CHellBoss_Bullet::CHellBoss_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CBullet_Base(pGraphic_Device)
{
}

CHellBoss_Bullet::CHellBoss_Bullet(const CHellBoss_Bullet& Prototype)
	:CBullet_Base(Prototype)
{
}

HRESULT CHellBoss_Bullet::Initialize_Prototype()
{
	return S_OK;
}


HRESULT CHellBoss_Bullet::Initialize(void* pArg)
{
	m_wBulletType = *reinterpret_cast<wstring*>(pArg);

	if (m_wBulletType == L"3_EyeBlast")
	{
		m_wBullet_Texture = L"Prototype_Component_Texture_HellBoss_Bullet";

		m_fFrameDuration = 0.02f;
		m_iFrameCount = 7;
		m_iMaxFrame = 7;
	}
	else if (m_wBulletType == L"4_Shoot")
	{
		m_wBullet_Texture = L"Prototype_Component_Texture_HellBoss_Hand_Bullet";

		m_fFrameDuration = 0.03f;
		m_iFrameCount = 10;
		m_iMaxFrame = 10;
	}


	m_fSpeed = 3.f;


	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_HellBoss_Transform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Instance()->Get_Component(LEVEL_HONG, TEXT("Layer_HellBoss"), TEXT("Com_Transform")));
	if (!m_HellBoss_Transform)
		return E_FAIL;


	m_fHellBoss_Pos = m_HellBoss_Transform->Get_State(CTransform::STATE_POSITION);




	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Instance()->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	if (pPlayerTransform)
	{
		_float3 vToPlayer = pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_fHellBoss_Pos;
		D3DXVec3Normalize(&vToPlayer, &vToPlayer);
		m_vDir = vToPlayer;
	}
	else
	{
		m_vDir = m_HellBoss_Transform->Get_State(CTransform::STATE_LOOK);
	}


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_fHellBoss_Pos);
	m_pTransformCom->Set_Scale(0.6f, 0.6f, 0.6f);

	return S_OK;
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
	D3DXVec3Normalize(&m_fHellBoss_Up, &m_fHellBoss_Up); // << 이거 꼭 추가

	if (!m_bInitializedPos)
	{
		_float3 offsetPos = m_fHellBoss_Pos;

		if (m_wBulletType == L"3_EyeBlast")
		{
			offsetPos += m_fHellBoss_Up * 4.2f;
			//offsetPos += m_fHellBoss_Look * 0.3f;
		}
		else if (m_wBulletType == L"4_Shoot")
		{
			offsetPos += m_fHellBoss_Up * 2.7f;
			//offsetPos += m_fHellBoss_Look * 1.5f;
		}

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, offsetPos);
		m_fHellBoss_Pos = offsetPos; // 이후 방향 계산에도 이 위치 써야함
		m_bInitializedPos = true;
	}

	// 방향 설정
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(
		m_pGameInstance->Get_Instance()->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));

	if (pPlayerTransform)
	{
		_float3 vToPlayer = pPlayerTransform->Get_State(CTransform::STATE_POSITION) - m_fHellBoss_Pos;
		D3DXVec3Normalize(&vToPlayer, &vToPlayer);
		m_vDir = vToPlayer;

		// 텍스처랑 스케일 설정
		if (m_wBulletType == L"3_EyeBlast")
		{
			m_wBullet_Texture = L"Prototype_Component_Texture_HellBoss_Bullet";
			m_fBullet_Scale = { 0.6f, 0.6f, 0.6f };
			m_fSpeed = 4.f;
		}
		else if (m_wBulletType == L"4_Shoot")
		{
			m_wBullet_Texture = L"Prototype_Component_Texture_HellBoss_Hand_Bullet";
			m_fBullet_Scale = { 3.f, 3.f, 3.f };
			m_fSpeed = 1.5f;
		}
	}
	else
	{
		m_vDir = m_fHellBoss_Look;
	}

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, m_fHellBoss_RIght);
	m_pTransformCom->Set_State(CTransform::STATE_UP, m_fHellBoss_Up);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_fHellBoss_Look);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_fHellBoss_Pos);
	m_pTransformCom->Set_Scale(m_fBullet_Scale.x, m_fBullet_Scale.y, m_fBullet_Scale.z);
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
	m_pTransformCom->Go(m_vDir, fTimeDelta * m_fSpeed);
	m_pParticleTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_float3 a = m_HellBoss_Transform->Get_State(CTransform::STATE_POSITION);
	_float3 b = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 dir = b - a;
	D3DXVec3Normalize(&dir, &dir);
	dynamic_cast<CProjectile_Particle_System*>(m_pParticleCom)->Set_Dir(m_vDir);
	m_pParticleCom->Update(fTimeDelta);
	m_fElapsedTime += fTimeDelta;

	if (m_fElapsedTime >= m_fFrameDuration)
	{
		m_fElapsedTime = 0.0f;

		if (m_iCurrentFrame < m_iMaxFrame)
		{
			m_iCurrentFrame = (m_iCurrentFrame + 1) % m_iFrameCount;
		}
		else
		{
			m_iCurrentFrame = m_iMaxFrame;
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


	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("총알 위치 X:") + to_wstring(m_pTransformCom->Get_WorldMat()._41),
		_float2(-300.f, 0.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("총알 위치 Y:") + to_wstring(m_pTransformCom->Get_WorldMat()._42),
		_float2(-100.f, 0.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("총알 위치 Z:") + to_wstring(m_pTransformCom->Get_WorldMat()._43),
		_float2(100.f, 0.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	return S_OK;
}

HRESULT CHellBoss_Bullet::On_Collision(CCollisionObject* other)
{
	if (nullptr == m_pColliderCom)
		return E_FAIL;

	if (nullptr == other)
		return S_OK;

	// 안바뀌면 충돌 안일어남
	if (other->Get_Type() == CG_END)
		return S_OK;

	_float3 fMTV = m_pColliderCom->Get_MTV();
	_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 temp = { 1.f, 0.f, 1.f };

	switch (other->Get_Type())
	{
	case CG_PLAYER:

		MSG_BOX("맞았음!");
		break;

	case CG_WEAPON:

		break;
	default:
		break;
	}


	return S_OK;
}


void CHellBoss_Bullet::Attack_Melee()
{
	m_pAttackCollider->Update_Collider(TEXT("Com_Transform"), m_pAttackCollider->Get_Scale());

	m_pGameInstance->Add_Collider(CG_PLAYER_PROJECTILE_SPHERE, m_pAttackCollider);
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

	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC	AttackColliderDesc = {};
	AttackColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	AttackColliderDesc.fScale = { 0.1f, 0.1f, 0.1f };
	// 오브젝트와 상대적인 거리 설정
	AttackColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackCollider), &AttackColliderDesc)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_HONG,m_wBullet_Texture,
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


	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 0.1f, 0.1f, 0.1f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };

	/* For.Com_Collider_Sphere */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
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

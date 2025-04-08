#include "HellBoss_Bullet.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"
#include "Particles.h"
#include "HellBoss.h"

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
	if (nullptr == pArg)
		return E_FAIL;


	 pDesc = *reinterpret_cast<PowerBlastDesc*>(pArg);

	 if (pDesc.wBulletType.empty())
		 return E_FAIL;


	 m_wBulletType = pDesc.wBulletType;
	if (pDesc.wBulletType == L"Power_Blast")
	{
		m_wBulletType = pDesc.wBulletType;
		m_fFixedAngle = (360.f / pDesc.iTotalCount) * pDesc.iIndex;
		m_fRotateAngle = m_fFixedAngle;
		m_eBulletMode = ROTATING; //

		m_iBulletIndex = pDesc.iIndex; 
		m_wBullet_Texture = L"Prototype_Component_Texture_HellBoss_Blast";
		m_fFrameDuration = 0.02f;
		m_iFrameCount = 7;
		m_iMaxFrame = 7;
	}
	else if (pDesc.wBulletType == L"3_EyeBlast")
	{
		m_wBullet_Texture = L"Prototype_Component_Texture_HellBoss_Bullet";
		m_fFrameDuration = 0.02f;
		m_iFrameCount = 7;
		m_iMaxFrame = 7;
	}
	else if (pDesc.wBulletType == L"4_Shoot")
	{
		m_wBullet_Texture = L"Prototype_Component_Texture_HellBoss_Hand_Bullet";
		m_fFrameDuration = 0.03f;
		m_iFrameCount = 10;
		m_iMaxFrame = 10;
	}
	else if (pDesc.wBulletType == L"0_Phase2_Shoot")
	{
		m_wBullet_Texture = L"Prototype_Component_Texture_HellBoss_Phase2_Hand_Bullet";
		m_fFrameDuration = 0.03f;
		m_iFrameCount = 6;
		m_iMaxFrame = 6;
	}


	m_vAxis = pDesc.vAxis; 

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

		if (m_wBulletType == L"3_EyeBlast") // 1페이즈
		{
			offsetPos += m_fHellBoss_Up * 4.2f;
		}
		else if (m_wBulletType == L"4_Shoot")//2페이즈
		{
			offsetPos += m_fHellBoss_Up * 2.7f;
		}
		else if (m_wBulletType == L"Power_Blast") // 보스 체력 기믹
		{
			static int iBlastIndex = 0;
			m_fRotateAngle = (360.f / 10.f) * iBlastIndex;
			++iBlastIndex;
			if (iBlastIndex >= 10)
				iBlastIndex = 0;
		}
		else if (m_wBulletType == L"0_Phase2_Shoot")//2페이즈			
		{
			offsetPos += m_fHellBoss_Up * 5.7f;
			offsetPos += m_fHellBoss_RIght * -0.14f;
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
			m_fBullet_Scale = { 0.6f, 0.6f, 0.6f };
			m_fSpeed = 4.f;
		}
		else if (m_wBulletType == L"4_Shoot")
		{
			m_fBullet_Scale = { 3.f, 3.f, 3.f };
			m_fSpeed = 1.5f;
		}
		else if (m_wBulletType == L"Power_Blast")
		{
			m_fBullet_Scale = { 2.f, 2.f, 2.f };
			m_fSpeed = 2.0f;
		}
		else if (m_wBulletType == L"0_Phase2_Shoot")
		{
			m_fBullet_Scale = { 2.f, 2.f, 2.f };
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

	m_bPlayedOnce = false; // 애니메이션 1회 재생
//
//	if (m_wBulletType == L"0_Phase2_Shoot")
//{
//    _float3 right = m_pTransformCom->Get_State(CTransform::STATE_RIGHT); // 지금 이걸로 좌우 반전 
//    m_pTransformCom->Rotation_Axis(right, D3DXToRadian(180.f));
//
//	_float3 Look = m_pTransformCom->Get_State(CTransform::STATE_LOOK); // 여기에서 시계방향으로 90도 회전
//	m_pTransformCom->Rotation_Axis(Look, D3DXToRadian(90.f));
//
//	_float3 Up = m_pTransformCom->Get_State(CTransform::STATE_UP); // 여기에서 시계방향으로 90도 회전
//	m_pTransformCom->Rotation_Axis(Up, D3DXToRadian(70.f));
//}


}

void CHellBoss_Bullet::Priority_Update(_float fTimeDelta)
{
	
	if (m_wBulletType == L"Power_Blast" && m_eBulletMode != LAUNCHING)
		return;

	m_fLifeTime += fTimeDelta;

	if (m_wBulletType != L"Power_Blast")
	{
		if (m_fLifeTime >= 2.f)
		{
			m_bIsActive = false;
			m_fLifeTime = 0.f;
		}
	}
	else
	{
		if (m_fLifeTime >= 5.f)
		{
			m_bIsActive = false;
			m_fLifeTime = 0.f;
		}

	}
}

void CHellBoss_Bullet::Update(_float fTimeDelta)
{

	if (pDesc.wBulletType == L"Power_Blast")
	{
		if (m_eBulletMode == ROTATING)
		{
			m_fRotateAngle += fTimeDelta * 90.f;

			D3DXMATRIX matRot;
			D3DXMatrixRotationAxis(&matRot, &m_vAxis, D3DXToRadian(m_fRotateAngle + m_fFixedAngle));

			_float3 offset = { m_fRadius, 0.f, 0.f }; // 기본 방향 (X축 기준)
			D3DXVec3TransformCoord(&offset, &offset, &matRot);

			_float3 bossPos = m_HellBoss_Transform->Get_State(CTransform::STATE_POSITION);
			_float3 newPos = bossPos + offset;

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, newPos);

			// LookAt 플레이어 처리
			CTransform* pPlayerTransform = dynamic_cast<CTransform*>(
				m_pGameInstance->Get_Instance()->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));

			if (pPlayerTransform)
			{
				_float3 vToPlayer = pPlayerTransform->Get_State(CTransform::STATE_POSITION) - newPos;
				vToPlayer.y = 0.f;
				vToPlayer.Normalize();

				_float3 vUp = _float3(0.f, 1.f, 0.f);
				_float3 vRight = vUp.Cross(vToPlayer);
				vRight.Normalize();

				_float3 vNewUp = vToPlayer.Cross(vRight);
				vNewUp.Normalize();

				m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
				m_pTransformCom->Set_State(CTransform::STATE_UP, vNewUp);
				m_pTransformCom->Set_State(CTransform::STATE_LOOK, vToPlayer);
			}

		}

		else if (m_eBulletMode == LAUNCHING)
		{
			if (m_eExpandPhase == EXPAND_SPREADING)
			{
				m_fExpandTime += fTimeDelta;

				_float lerpRatio = min(m_fExpandTime / 1.f, 1.f); // 1초간 퍼짐

				_float3 curPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_float3 lerpPos = Lerp(curPos, m_vExpandedPos, lerpRatio);
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, lerpPos);

				if (lerpRatio >= 1.f)
				{
					m_eExpandPhase = EXPAND_LAUNCH;

					// 플레이어 방향 재계산
					CTransform* pPlayerTransform = dynamic_cast<CTransform*>(
						m_pGameInstance->Get_Instance()->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));

					if (pPlayerTransform)
					{
						_float3 vToPlayer = pPlayerTransform->Get_State(CTransform::STATE_POSITION) - lerpPos;
						D3DXVec3Normalize(&vToPlayer, &vToPlayer);
						m_vDir = vToPlayer;
					}
				}
			}
			else if (m_eExpandPhase == EXPAND_LAUNCH)
			{
				// 플레이어 향해 날아가기
				m_pTransformCom->Go(m_vDir, fTimeDelta * m_fSpeed);
			}
		}


	}

	m_pTransformCom->Go(m_vDir, fTimeDelta * m_fSpeed);
	m_pTransformCom->Go(m_vDir, fTimeDelta * m_fSpeed);
	m_pParticleTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_float3 a = m_HellBoss_Transform->Get_State(CTransform::STATE_POSITION);
	_float3 b = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 dir = b - a;
	D3DXVec3Normalize(&dir, &dir);


	if (m_wBulletType == L"Power_Blast" && m_eBulletMode == LAUNCHING)
	{
		dynamic_cast<CProjectile_Particle_System*>(m_pParticleCom)->Set_Dir(m_vDir);
		m_pParticleCom->Update(fTimeDelta);
	}
	else if (m_wBulletType != L"Power_Blast")
	{

		dynamic_cast<CProjectile_Particle_System*>(m_pParticleCom)->Set_Dir(m_vDir);
		m_pParticleCom->Update(fTimeDelta);
	}



	m_fElapsedTime += fTimeDelta;

	if (m_fElapsedTime >= m_fFrameDuration)
	{
		m_fElapsedTime = 0.0f;

		if (m_wBulletType == L"Power_Blast")
		{
			if (m_eBulletMode == LAUNCHING)
			{
				if (!m_bPlayedOnce)
				{
					if (m_iCurrentFrame < m_iMaxFrame - 2)
					{
						++m_iCurrentFrame;
					}
					else
					{
						m_iCurrentFrame = m_iMaxFrame - 2;
						m_bPlayedOnce = true;
					}
				}
				// else { 멈춤 유지 }
			}
			else // ROTATING 상태
			{
				m_iCurrentFrame = (m_iCurrentFrame + 1) % m_iFrameCount;
			}
		}
		else
		{
			// 다른 총알은 루프 애니메이션
			m_iCurrentFrame = (m_iCurrentFrame + 1) % m_iFrameCount;
		}
	}




}
_float3 CHellBoss_Bullet::Lerp(const _float3& a, const _float3& b, _float t)
{
	return a + (b - a) * t;
}

void CHellBoss_Bullet::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pAttackCollider->Update_Collider_Boss(TEXT("Com_Transform"));

	m_pGameInstance->Add_Collider(CG_MONSTER_PROJECTILE_SPHERE, m_pColliderCom);
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
	if (m_wBulletType != L"Power_Blast" || m_eBulletMode == LAUNCHING)
	{
		if (FAILED(m_pParticleTransformCom->Bind_Resource()))
			return E_FAIL;
		if (FAILED(m_pParticleCom->Render()))
			return E_FAIL;
	}


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

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_HONG, m_wBullet_Texture,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC	AttackColliderDesc = {};
	AttackColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	AttackColliderDesc.fScale = { 10.f, 10.f, 10.f };
	// 오브젝트와 상대적인 거리 설정
	AttackColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackCollider), &AttackColliderDesc)))
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
	AttackColliderDesc.fScale = { 10.f, 10.f, 10.f };
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

void CHellBoss_Bullet::Launch_Toward_Player()
{
	if (!m_HellBoss_Transform)
		return;

	m_eBulletMode = LAUNCHING;
	m_eExpandPhase = EXPAND_SPREADING;
	m_fExpandTime = 0.f;


	_float3 bossPos = m_HellBoss_Transform->Get_State(CTransform::STATE_POSITION);
	_float3 lookDir = m_HellBoss_Transform->Get_State(CTransform::STATE_LOOK);
	_float3 rightDir = m_HellBoss_Transform->Get_State(CTransform::STATE_RIGHT);
	_float3 upDir = m_HellBoss_Transform->Get_State(CTransform::STATE_UP);

	D3DXVec3Normalize(&lookDir, &lookDir);
	D3DXVec3Normalize(&rightDir, &rightDir);
	D3DXVec3Normalize(&upDir, &upDir);

	//  보스 뒤로 약간 떨어진 위치하고싶엉
	_float3 centerPos = bossPos - lookDir * 10.f;

	// (5x2 형태로, X: 가로, Y: 세로)
	const _int row = m_iBulletIndex / 5; // 0 또는 1
	const _int col = m_iBulletIndex % 5; // 0 ~ 4

	const _float spreadX = 10.f; // 좌우 퍼짐 
	const _float spreadY = 4.f;  // 상하 퍼짐 

	_float xOffset = (col - 2) * spreadX;      // -20 ~ +20
	_float yOffset = (row - 0.5f) * spreadY;   // -2 ~ +2

	// 중심 + 좌우 + 위아래
	m_vExpandedPos = centerPos + rightDir * xOffset + upDir * yOffset;
}

#include "Staff_Bullet.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"
#include "Particles.h"

CStaff_Bullet::CStaff_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CBullet_Base(pGraphic_Device)
{
}

CStaff_Bullet::CStaff_Bullet(const CStaff_Bullet& Prototype)
	:CBullet_Base( Prototype )
{
}

HRESULT CStaff_Bullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStaff_Bullet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_fSpeed = 5.f;
	m_vDir = m_Player_Transform->Get_State(CTransform::STATE_LOOK);

	m_iAp = 30;

	m_eType = CG_PLAYER_PROJECTILE_SPHERE;

	return S_OK;
}

void CStaff_Bullet::Priority_Update(_float fTimeDelta)
{
	m_fLifeTime += fTimeDelta;

	if (m_fLifeTime >= 2.f)
	{
		m_bIsActive = false;
		m_fLifeTime = 0.f;
	}
}

void CStaff_Bullet::Update(_float fTimeDelta)
{
	
	m_pTransformCom->Go(m_vDir, fTimeDelta * m_fSpeed);
	m_pParticleTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	dynamic_cast<CProjectile_Particle_System*>(m_pParticleCom)->Set_Dir(m_vDir);
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
	m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pColliderCom->Get_Scale());

	m_pGameInstance->Add_Collider(CG_PLAYER_PROJECTILE_SPHERE, m_pColliderCom);
}



void CStaff_Bullet::Late_Update(_float fTimeDelta)
{	
	__super::Late_Update(fTimeDelta);
}


HRESULT CStaff_Bullet::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	m_pShaderCom->Bind_Transform();
	m_pShaderCom->Bind_Texture(m_pTextureCom, m_iCurrentFrame);
	m_pShaderCom->Bind_Material(m_pMaterialCom);

	m_pShaderCom->Begin(0);
	SetUp_RenderState();
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	m_pShaderCom->End();
	if (FAILED(m_pParticleTransformCom->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_pParticleCom->Render()))
		return E_FAIL;

	Release_RenderState();
	return S_OK;
}

HRESULT CStaff_Bullet::On_Collision(CCollisionObject* other)
{
	if (nullptr == m_pColliderCom)
		return E_FAIL;

	if (nullptr == other)
		return S_OK;

	// 안바뀌면 충돌 안일어남
	if (other->Get_Type() == CG_END)
		return S_OK;

	switch (other->Get_Type())
	{
	case CG_PLAYER:

		
		break;

	case CG_WEAPON:

		break;
	case CG_MONSTER:
		Take_Damage(other);
		m_bIsActive = false;
		break;

	case CG_STRUCTURE_WALL:
		m_bIsActive = false;
		break;

	case CG_DOOR:
		m_bIsActive = false;
		break;

	
	default:
		break;
	}



	return S_OK;

}


void CStaff_Bullet::Attack_Melee()
{

}

void CStaff_Bullet::Reset()
{
	m_fElapsedTime = 0.f;
	m_iCurrentFrame = 0;

	if (!m_Player_Transform) return;

	Player_RIght = m_Player_Transform->Get_State(CTransform::STATE_RIGHT);
	Player_Up = m_Player_Transform->Get_State(CTransform::STATE_UP);
	Player_Look = m_Player_Transform->Get_State(CTransform::STATE_LOOK);
	Player_Pos = m_Player_Transform->Get_State(CTransform::STATE_POSITION);


	/*Player_Pos += Player_Look * 0.8f;
	Player_Pos += Player_RIght * 0.1f;
	Player_Pos.y -= 0.5f;*/

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, Player_RIght);
	m_pTransformCom->Set_State(CTransform::STATE_UP, Player_Up);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, Player_Look);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Player_Pos);
	m_pTransformCom->Set_Scale(0.6f, 0.6f, 0.6f);
	m_vDir = m_Player_Transform->Get_State(CTransform::STATE_LOOK);


}

HRESULT CStaff_Bullet::SetUp_RenderState()
{
	D3DXVECTOR2 vScaleFactor(1.f, 1.f);
	D3DXVECTOR2 vOffsetFactor(0.0f, 0.0f); // Y축 반전을 위한 오프셋 조정
	m_pShaderCom->Set_UVScaleFactor(&vScaleFactor);
	m_pShaderCom->Set_UVOffsetFactor(&vOffsetFactor);

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)
	return S_OK;
}

HRESULT CStaff_Bullet::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CStaff_Bullet::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Weapon_Bullet"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	CProjectile_Particle_System::TRAILDESC     trailDesc{};
	trailDesc.fDistance = 30.f;
	trailDesc.fWidth = 0.5f;
	trailDesc.iNumParticles = 1;
	trailDesc.strTexturePath = L"../../Resources/Textures/Particle/sprite_blood_particle.png";
	trailDesc.iNumTextures = 1;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Particle_Projectile"),
		TEXT("Com_Particle"), reinterpret_cast<CComponent**>(&m_pParticleCom), &trailDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_ParticleTransform"), reinterpret_cast<CComponent**>(&m_pParticleTransformCom))))
		return E_FAIL;

	/* For.Com_Material */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BaseShader"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 1.f, 1.f, 1.f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.5f, 0.f };


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

CStaff_Bullet* CStaff_Bullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CStaff_Bullet* pInstance = new CStaff_Bullet(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStaff_Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStaff_Bullet::Clone(void* pArg)
{
	CStaff_Bullet* pInstance = new CStaff_Bullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CStaff_Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaff_Bullet::Free()
{
	__super::Free();

	Safe_Release(m_pLightCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMaterialCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pAttackCollider);
	Safe_Release(m_pParticleCom);
	Safe_Release(m_pParticleTransformCom);
}

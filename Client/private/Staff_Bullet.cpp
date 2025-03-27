#include "Staff_Bullet.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"

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
	return S_OK;
}

void CStaff_Bullet::Priority_Update(_float fTimeDelta)
{

}

void CStaff_Bullet::Update(_float fTimeDelta)
{
	
	m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed);

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

	SetUp_RenderState();

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	Release_RenderState();

	return S_OK;
}

HRESULT CStaff_Bullet::On_Collision(CCollisionObject* other)
{
	__super::On_Collision(other);

	

	return S_OK;
}


void CStaff_Bullet::Attack_Melee()
{
	m_pAttackCollider->Update_Collider(TEXT("Com_Transform"), m_pAttackCollider->Get_Scale());

	m_pGameInstance->Add_Collider(CG_MONSTER_PROJECTILE_CUBE, m_pAttackCollider);
}

HRESULT CStaff_Bullet::SetUp_RenderState()
{
	// 일단 추가해보기

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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Weapon_Bullet"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
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

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pAttackCollider);
}

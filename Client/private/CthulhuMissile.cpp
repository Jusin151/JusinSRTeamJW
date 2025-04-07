#include "Texture.h"
#include "CthulhuMissile.h"
#include "VIBuffer_Rect.h"
#include "Collider_Sphere.h"
#include "GameInstance.h"
#include <Player.h>


CCthulhuMissile::CCthulhuMissile(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CProjectile_Base(pGraphic_Device)
{
}

CCthulhuMissile::CCthulhuMissile(const CCthulhuMissile& Prototype)
	: CProjectile_Base(Prototype)
{  m_iAp = Prototype.m_iAp;
	m_bIsColned = true;
	m_pTransformCom = { nullptr };
}


HRESULT CCthulhuMissile::Initialize_Prototype()
{
	m_iAp = 10;
	return S_OK;
}

HRESULT CCthulhuMissile::Initialize(void* pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_pTarget = static_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_Player"));
	Safe_AddRef(m_pTarget);
	if (nullptr != pArg)
	{
		PROJ_DESC* pDesc = static_cast<PROJ_DESC*>(pArg);

		m_vPos = pDesc->vPos;
		m_vDir = pDesc->vDir;
		m_fSpeed = pDesc->fSpeed;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPos);
	}
	m_eType = CG_MONSTER_PROJECTILE_CUBE;
	return S_OK;
}

void CCthulhuMissile::Priority_Update(_float fTimeDelta)
{
	Move(fTimeDelta);
}

void CCthulhuMissile::Update(_float fTimeDelta)
{
	if (m_pColliderCom)
	{
		m_pColliderCom->Set_WorldMat(m_pTransformCom->Get_WorldMat());

		m_pColliderCom->Update_Collider(TEXT("Com_Collider_Cube"), m_pTransformCom->Compute_Scaled());
		m_pGameInstance->Add_Collider(CG_MONSTER_PROJECTILE_CUBE, m_pColliderCom);
	}
	Update_Animation(fTimeDelta);
	Billboarding(fTimeDelta);
}

void CCthulhuMissile::Late_Update(_float fTimeDelta)
{


	m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CCthulhuMissile::Render()
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

HRESULT CCthulhuMissile::On_Collision(CCollisionObject* other)
{
	if (nullptr == other)
		return E_FAIL;
	switch (other->Get_Type())
	{
	case CG_PLAYER:
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

void CCthulhuMissile::Reset()
{
	m_fFrame = 0.f;
	m_iCurrentFrame = 0;
	m_bIsHit = false;
	m_bIsDead = false;
}

HRESULT CCthulhuMissile::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	return S_OK;
}

HRESULT CCthulhuMissile::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}
HRESULT CCthulhuMissile::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;	
	if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Texture_CthulhuMissile"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;

	ColliderDesc.fScale = { 1.f, 1.f, 1.f };
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;
	return S_OK;
}

void CCthulhuMissile::Billboarding(_float fTimeDelta)
{
	if (!m_pTarget || !m_pTransformCom) return;

	m_pTransformCom->LookAt(m_pTarget->Get_TransForm()->Get_State(CTransform::STATE_POSITION));
}

void CCthulhuMissile::Update_Animation(_float fTimeDelta)
{
	m_fFrame += m_fAnimationSpeed * fTimeDelta;
	if (m_fFrame >= m_iMaxFrame)
	{
		m_fFrame = 0.f;
	}
	m_iCurrentFrame = (_uint)m_fFrame;
}

void CCthulhuMissile::Move(_float fTimeDelta)
{
	_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	fPos += m_vDir * m_fSpeed * fTimeDelta;

 	m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
}

CCthulhuMissile* CCthulhuMissile::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCthulhuMissile* pInstance = new CCthulhuMissile(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCthulhuMissile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCthulhuMissile::Clone(void* pArg)
{
	CCthulhuMissile* pInstance = new CCthulhuMissile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CCthulhuMissile");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCthulhuMissile::Free()
{
	__super::Free();
	if (!m_bIsColned)
	{

	}
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTarget);
}

#include "GlacierBullet.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Collider_Cube.h"
#include "Player.h"

CGlacierBullet::CGlacierBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CProjectile_Base(pGraphic_Device)
{
}

CGlacierBullet::CGlacierBullet(const CGlacierBullet& Prototype)
    :CProjectile_Base(Prototype)
{
}

HRESULT CGlacierBullet::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGlacierBullet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		PROJ_DESC* pDesc = static_cast<PROJ_DESC*>(pArg);

		m_vPos = pDesc->vPos;
		m_vDir = pDesc->vDir;
		m_fSpeed = pDesc->fSpeed;


		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPos);
	}


	m_eType = CG_MONSTER_PROJECTILE_CUBE;
	m_iAp = 5;


	return S_OK;
}

void CGlacierBullet::Priority_Update(_float fTimeDelta)
{
}

void CGlacierBullet::Update(_float fTimeDelta)
{

	_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	fPos += m_vDir * m_fSpeed * fTimeDelta;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);

	CPlayer* m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player")));

	m_pTransformCom->LookAt(m_pPlayer->Get_TransForm()->Get_State(CTransform::STATE_POSITION));

	m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pColliderCom->Get_Scale());

	m_pGameInstance->Add_Collider(CG_MONSTER_PROJECTILE_CUBE, m_pColliderCom);

}

void CGlacierBullet::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CGlacierBullet::Render()
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


	return S_OK;
}

HRESULT CGlacierBullet::On_Collision(CCollisionObject* other)
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

HRESULT CGlacierBullet::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)

	return S_OK;
}

HRESULT CGlacierBullet::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CGlacierBullet::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GlacierBullet"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 0.5f, 0.5f, 0.5f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CGlacierBullet* CGlacierBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGlacierBullet* pInstance = new CGlacierBullet(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGlacierBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGlacierBullet::Clone(void* pArg)
{
	CGlacierBullet* pInstance = new CGlacierBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CGlacierBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGlacierBullet::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
}

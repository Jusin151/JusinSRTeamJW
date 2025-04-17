#include "LookerBullet.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Collider_Sphere.h"
#include "Player.h"

CLookerBullet::CLookerBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CProjectile_Base(pGraphic_Device)
{
}

CLookerBullet::CLookerBullet(const CLookerBullet& Prototype)
    :CProjectile_Base(Prototype)
{
}

HRESULT CLookerBullet::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLookerBullet::Initialize(void* pArg)
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

	m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);

	m_eType = CG_MONSTER_PROJECTILE_SPHERE;
	m_iAp = 5;


	return S_OK;
}

void CLookerBullet::Priority_Update(_float fTimeDelta)
{
	m_fLifeTime += fTimeDelta;

	if (m_fLifeTime >= 4.f)
	{
		m_bIsActive = false;
		m_fLifeTime = 0.f;
	}
}

void CLookerBullet::Update(_float fTimeDelta)
{
	_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	fPos += m_vDir * m_fSpeed * fTimeDelta;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);

	CPlayer* m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player")));

	m_pTransformCom->LookAt(m_pPlayer->Get_TransForm()->Get_State(CTransform::STATE_POSITION));

	m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pColliderCom->Get_Scale());

	m_pGameInstance->Add_Collider(CG_MONSTER_PROJECTILE_SPHERE, m_pColliderCom);
}

void CLookerBullet::Late_Update(_float fTimeDelta)
{
	Update_Frame(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CLookerBullet::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, m_iCurrentFrame)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Transform()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Lights()))
		return E_FAIL;

	SetUp_RenderState();
	m_pShaderCom->Begin(0);
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	m_pShaderCom->End();
	Release_RenderState();


	return S_OK;
}

HRESULT CLookerBullet::On_Collision(CCollisionObject* other)
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
		fPos -= m_vDir.GetNormalized();
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
		CreateParticle(0xFF55aa44, 0x0055aa44);
		m_bIsActive = false;
		break;
	case CG_DOOR:
		fPos -= m_vDir.GetNormalized();
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
		CreateParticle(0xFF55aa44, 0x0055aa44);
		m_bIsActive = false;
		break;
	default:
		break;
	}


	return S_OK;
}

void CLookerBullet::Update_Frame(_float fTimeDelta)
{

	m_fElapsedTime += fTimeDelta;

	if (m_fElapsedTime >= 0.1f)
	{
		m_fElapsedTime = 0.f;
		m_iCurrentFrame = (m_iCurrentFrame +1) % 8;
	}

}

HRESULT CLookerBullet::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)

	return S_OK;
}

HRESULT CLookerBullet::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CLookerBullet::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LookerBullet"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 0.5f, 0.5f, 0.5f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CLookerBullet* CLookerBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLookerBullet* pInstance = new CLookerBullet(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLookerBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLookerBullet::Clone(void* pArg)
{
	CLookerBullet* pInstance = new CLookerBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CLookerBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLookerBullet::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
}

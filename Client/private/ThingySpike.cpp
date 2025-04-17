#include "ThingySpike.h"
#include "GameInstance.h"
#include "Player.h"

CThingySpike::CThingySpike(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CProjectile_Base(pGraphic_Device)
{
}

CThingySpike::CThingySpike(const CThingySpike& Prototype)
	:CProjectile_Base(Prototype)
{
}

HRESULT CThingySpike::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CThingySpike::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		PROJ_DESC* pDesc = static_cast<PROJ_DESC*>(pArg);

		m_vPos = pDesc->vPos;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPos);
	}

	m_vCurPos = m_vPos;

	m_vNextPos = m_vPos;

	m_pTransformCom->Set_Scale(1.f, 1.5f, 1.f);

	m_pColliderCom->Set_Scale(_float3(1.f, 0.15f, 1.f));

	

	m_eType = CG_MONSTER_PROJECTILE_CUBE;
	m_iAp = 10;


	return S_OK;
}

void CThingySpike::Priority_Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
	{
		m_pTarget = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"));
		if (nullptr == m_pTarget)
			return;

		
		Safe_AddRef(m_pTarget);
	}

	if (m_iCurrentFrame >= 20)
		m_bIsActive = false;
}

void CThingySpike::Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;

	Billboarding(fTimeDelta);

	if(m_iCurrentFrame > 10)
		m_pColliderCom->Set_Scale(_float3(1.5f, 0.15f * ((m_iCurrentFrame % 10)+ 1), 1.5f));
	else
		m_pColliderCom->Set_Scale(_float3(1.5f, 1.5f - 0.15f * ((m_iCurrentFrame % 10) + 1), 1.5f));

	m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pColliderCom->Get_Scale());

	m_pGameInstance->Add_Collider(CG_MONSTER_PROJECTILE_CUBE, m_pColliderCom);
}

void CThingySpike::Late_Update(_float fTimeDelta)
{

	if (nullptr == m_pTarget)
		return;

	m_fElapsedTime += fTimeDelta;

	if (m_fElapsedTime >= 0.1f)
	{
		m_fElapsedTime = 0.f;
		++m_iCurrentFrame;
	}

	Calc_Position();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vCurPos);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CThingySpike::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Transform()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, m_iCurrentFrame)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
		return E_FAIL;

	SetUp_RenderState();
	m_pShaderCom->Begin(0);
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	m_pShaderCom->End();
	Release_RenderState();


	return S_OK;
}

HRESULT CThingySpike::On_Collision(CCollisionObject* other)
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
		if(!m_bIsHit)
			Take_Damage(other);
		m_bIsHit = true;

		break;
	case CG_STRUCTURE_WALL:
	
		break;
	case CG_DOOR:
		
		break;
	default:
		break;
	}


	return S_OK;

    return S_OK;
}

HRESULT CThingySpike::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)

	return S_OK;
}

HRESULT CThingySpike::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CThingySpike::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ThingySpike"),
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

void CThingySpike::Billboarding(_float fTimeDelta)
{
    if (TEXT("Layer_Player") != m_pTarget->Get_Tag())
        return;

    m_pTransformCom->LookAt(static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION));
}


CThingySpike* CThingySpike::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CThingySpike* pInstance = new CThingySpike(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CThingySpike");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CThingySpike::Clone(void* pArg)
{
	CThingySpike* pInstance = new CThingySpike(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CThingySpike");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CThingySpike::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTarget);
}

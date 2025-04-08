#include "Anubis.h"
#include "Monster_Base.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Collider_Cube.h"
#include "Transform.h"
#include "Player.h"
#include "GameInstance.h"

CAnubis::CAnubis(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CMonster_Base(pGraphic_Device)
{
}

CAnubis::CAnubis(const CAnubis& Prototype)
	:CMonster_Base(Prototype)
{
}

HRESULT CAnubis::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnubis::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(20.f, 1.f, 20.f));

	m_eType = CG_MONSTER;

	return S_OK;
}

void CAnubis::Priority_Update(_float fTimeDelta)
{
	
}

void CAnubis::Update(_float fTimeDelta)
{
	Select_Pattern(fTimeDelta);

	__super::Update(fTimeDelta);

	m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pColliderCom->Get_Scale());

	m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
}

void CAnubis::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CAnubis::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(0)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	SetUp_RenderState();
	if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, m_iCurrentFrame)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Transform()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	m_pShaderCom->End();

	Release_RenderState();


	return S_OK;
}

HRESULT CAnubis::On_Collision(CCollisionObject* other)
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

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
		//m_pTransformCom->Go_Backward(fTimeDelta);
		break;

	case CG_WEAPON:



		temp += fPos;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, temp);

		break;
	default:
		break;
	}


	return S_OK;
}

void CAnubis::Select_Pattern(_float fTimeDelta)
{
	_float3 fDist;
	fDist = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION);
	if (fDist.LengthSq() > 5)
		m_pTransformCom->Chase(static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION), fTimeDelta * 0.25f);
	else
		Attack_Melee();
}

void CAnubis::Attack_Melee()
{
	m_pAttackCollider->Update_Collider(TEXT("Com_Transform"), m_pColliderCom->Get_Scale());

	m_pGameInstance->Add_Collider(CG_MONSTER_PROJECTILE_CUBE, m_pAttackCollider);
}

HRESULT CAnubis::SetUp_RenderState()
{
	// 일단 추가해보기

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)
	_float2 ScaleFactor = { 1.0f, 1.0f };
	_float2 Offset = { 0.f, 0.f };
	m_pShaderCom->Set_UVScaleFactor(&ScaleFactor);
	m_pShaderCom->Set_UVOffsetFactor(&Offset);
	return S_OK;
}

HRESULT CAnubis::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CAnubis::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Anubis"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	_float3 fPos = { 10.f, 0.5f, 10.f };

	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 3.f, 1.f, 3.f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.5f, 0.f };


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackCollider), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CAnubis* CAnubis::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CAnubis* pInstance = new CAnubis(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAnubis");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAnubis::Clone(void* pArg)
{
	CAnubis* pInstance = new CAnubis(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CAnubis");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnubis::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pAttackCollider);
}

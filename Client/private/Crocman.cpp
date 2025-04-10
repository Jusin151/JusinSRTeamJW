#include "Crocman.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Collider_Cube.h"
#include "Transform.h"
#include "Player.h"
#include "GameInstance.h"

CCrocman::CCrocman(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CMonster_Base(pGraphic_Device)
{
}

CCrocman::CCrocman(const CCrocman& Prototype)
	:CMonster_Base(Prototype)
{
}

HRESULT CCrocman::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCrocman::Initialize(void* pArg)
{
	INIT_PARENT(pArg)
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_eType = CG_MONSTER;

	m_iAp = 5;

	m_iHp = 30;

	m_fSpeed = 0.15f;

	m_pColliderCom->Set_Scale(_float3(2.f, 2.f, 2.f));

	return S_OK;
}

void CCrocman::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (m_iCurrentFrame > 26)
	{
		m_bIsActive = false;
		
	}
}

void CCrocman::Update(_float fTimeDelta)
{
	if (m_pTarget == nullptr)
		return;
	if (!m_bCheck)
	{
		m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
		return;
	}


	Select_Pattern(fTimeDelta);



	__super::Update(fTimeDelta);


	if (m_eCurState != MS_DEATH)
	{
		m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pTransformCom->Compute_Scaled());

		m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
	}

}

void CCrocman::Late_Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;

	Select_Frame(fTimeDelta);

	if (m_pGameInstance->IsPointInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
			return;
	}
}

HRESULT CCrocman::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
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

	if (g_bDebugCollider)
	{
		m_pColliderCom->Render();
	}
	m_pShaderCom->End();
		
	Release_RenderState();


	return S_OK;
}

void CCrocman::Deserialize(const json& j)
{
	SET_TRANSFORM(j, m_pTransformCom);
}

HRESULT CCrocman::On_Collision(CCollisionObject* other)
{
	if (nullptr == m_pColliderCom)
		return E_FAIL;
	if (nullptr == other)
		return S_OK;
	
	// 안바뀌면 충돌 안일어남
	if (other->Get_Type() == CG_END)
		return S_OK;

	
	_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vMtv = m_pColliderCom->Get_MTV();
	_float3 vMove = { vMtv.x, 0.f, vMtv.z };

	switch (other->Get_Type())
	{
	case CG_PLAYER:

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
		//m_pTransformCom->Go_Backward(fTimeDelta);
	

		if (m_eCurState != MS_ATTACK)
		{
			Take_Damage(other);
			
			
		}
		else
		{
			m_iAp *= 3;
			Take_Damage(other);
			m_iAp /= 3;
		}
		
		
		break;

	case CG_WEAPON:
		Create_Stains(5);
		

		m_eCurState = MS_HIT;

		break;

	case CG_MONSTER:
		m_vObjectMtvSum += vMove * 0.5f;

		break;
	case CG_STRUCTURE_WALL:
	
		break;
	case CG_DOOR:

		break;
	default:
		break;
	}

	

	return S_OK;
}

void CCrocman::Select_Pattern(_float fTimeDelta)
{
	if (m_eCurState == MS_DEATH)
		return;

	_float3 vDist;
	vDist = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION);


	switch (m_eCurState)
	{
	case MS_IDLE:
		if (vDist.LengthSq() > 10)
		{
			if (m_eCurState != MS_WALK)
				m_eCurState = MS_WALK;
		
		}
		else
		{
			m_eCurState = MS_ATTACK;
		}
		break;
	case MS_WALK:
		Chasing(fTimeDelta, m_pColliderCom->Get_Scale().Length());
		break;
	case MS_HIT:
		// 맞고 바로 안바뀌도록

		if (m_fElapsedTime >= 0.5f)
			m_eCurState = MS_IDLE;
		else
			return;

		break;
	case MS_ATTACK:
		Attack_Melee(fTimeDelta);

		break;

	default:
		break;
	}


	
}

void CCrocman::Attack_Melee(_float fTimeDelta)
{
	if (m_eCurState != MS_ATTACK)
	{
		if (m_fElapsedTime >= 1.f)
			m_eCurState = MS_ATTACK;
		else
			return;
	}

}

void CCrocman::Select_Frame(_float fTimeDelta)
{
	m_fElapsedTime += fTimeDelta;

	

	switch (m_eCurState)
	{
	case MS_IDLE:
		m_iCurrentFrame = 0;
		break;
	case MS_WALK:

		if (m_iCurrentFrame == 8)
		{
			m_eCurState = MS_IDLE;

		}

		if (m_iCurrentFrame < 2 || m_iCurrentFrame > 8)
			m_iCurrentFrame = 2;

		if (m_fElapsedTime >= 0.2f)
		{
			m_fElapsedTime = 0.0f;

			m_iCurrentFrame++;

		}
		break;
	case MS_HIT:
		m_iCurrentFrame = 1;
		break;
	case MS_ATTACK:

		if (m_iCurrentFrame == 17)
		{
			m_eCurState = MS_IDLE;
		}

		if (m_iCurrentFrame < 9 || m_iCurrentFrame > 17)
			m_iCurrentFrame = 9;

		if (m_fElapsedTime >= 0.2f)
		{
			m_fElapsedTime = 0.0f;

			m_iCurrentFrame++;

		}
		break;
	case MS_DEATH:
		if (m_iCurrentFrame < 18)
			m_iCurrentFrame = 18;

		if (m_fElapsedTime >= 0.2f)
		{
			m_fElapsedTime = 0.0f;

			m_iCurrentFrame++;

		}
		break;
	
	default:
		break;
	}
}

HRESULT CCrocman::SetUp_RenderState()
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

HRESULT CCrocman::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CCrocman::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_tObjDesc.iLevel,  m_tObjDesc.stProtTextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	_float3 fPos = { 10.f, 0.5f, 10.f };

	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 1.f, 1.f, 1.f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackCollider), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CCrocman* CCrocman::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCrocman* pInstance = new CCrocman(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCrocman");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCrocman::Clone(void* pArg)
{
	CCrocman* pInstance = new CCrocman(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCrocman");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCrocman::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pAttackCollider);
	Safe_Release(m_pTarget);
}

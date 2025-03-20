#include "Crocman.h"
#include "Monster_Base.h"
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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(20.f, 1.f, 20.f));

	return S_OK;
}

void CCrocman::Priority_Update(_float fTimeDelta)
{


	if (m_iCurrentFrame > 26)
	{
		m_bIsActive = false;
		
	}
}

void CCrocman::Update(_float fTimeDelta)
{
	Select_Pattern(fTimeDelta);

	__super::Update(fTimeDelta);


	if (m_eCurState != MS_DEATH)
	{
		m_pColliderCom->Update_Collider(TEXT("Com_Transform"));

		m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
	}

}

void CCrocman::Late_Update(_float fTimeDelta)
{
	// 충돌 판정
	On_Collision(fTimeDelta);


	Select_Frame(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RG_COLLIDER, this);
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CCrocman::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	//SetUp_RenderState();

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	Release_RenderState();


	return S_OK;
}

HRESULT CCrocman::On_Collision(_float fTimeDelta)
{
	if (nullptr == m_pColliderCom)
		return E_FAIL;

	
	// 안바뀌면 충돌 안일어남
	if (m_pColliderCom->Get_Other_Type() == CG_END)
		return S_OK;

	_float3 fMTV = m_pColliderCom->Get_MTV();
	_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 temp = { 1.f, 0.f, 1.f };

	switch (m_pColliderCom->Get_Other_Type())
	{
	case CG_PLAYER:

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
		//m_pTransformCom->Go_Backward(fTimeDelta);

		m_eCurState = MS_HIT;
		break;

	case CG_WEAPON:

		temp += fPos;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, temp);

		m_eCurState = MS_DEATH;

		break;
	default:
		break;
	}

	// 충돌 처리 하고 다시 type을 수정
	m_pColliderCom->Set_Other_Type(CG_END);

	return S_OK;
}

void CCrocman::Select_Pattern(_float fTimeDelta)
{
	if (m_eCurState == MS_DEATH)
		return;

	_float3 vDist;
	//vDist = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION);

	// 거리로 판단해서 패턴 실행하도록 
	if (vDist.LengthSq() > 5)
		Chasing(fTimeDelta);
	else
		Attack_Melee(fTimeDelta);
}

void CCrocman::Chasing(_float fTimeDelta)
{
	// 맞고 바로 안하도록
	if (m_eCurState == MS_HIT)
	{
		if (m_fElapsedTime >= 1.f)
			m_eCurState = MS_WALK;
		else
			return;
	}

	else if (m_eCurState != MS_WALK)
		m_eCurState = MS_WALK;

	m_pTransformCom->Chase(static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION), fTimeDelta * 0.25f);
}

void CCrocman::Attack_Melee(_float fTimeDelta)
{
	if (m_eCurState == MS_IDLE)
	{
		if (m_fElapsedTime >= 1.f)
			m_eCurState = MS_ATTACK;
		else
			return;
	}


	if (m_eCurState != MS_ATTACK)
		m_eCurState = MS_ATTACK;

	m_pAttackCollider->Update_Collider(TEXT("Com_Transform"));

	// 일단 투사체 판정으로 해놓고 나중에 다른 enum 사용하면 될듯?
	m_pGameInstance->Add_Collider(CG_MONSTER_PROJECTILE, m_pAttackCollider);
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

		if (m_fElapsedTime >= 0.3f)
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
	ColliderDesc.eType = CG_MONSTER_PROJECTILE;
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
}

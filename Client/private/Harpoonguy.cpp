#include "Harpoonguy.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Collider_Cube.h"
#include "Transform.h"
#include "Player.h"
#include "GameInstance.h"
#include "Harpoon.h"
#include "Projectile_Base.h"

CHarpoonguy::CHarpoonguy(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CMonster_Base(pGraphic_Device)
{
}

CHarpoonguy::CHarpoonguy(const CHarpoonguy& Prototype)
	:CMonster_Base(Prototype)
{
}

HRESULT CHarpoonguy::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHarpoonguy::Initialize(void* pArg)
{
	INIT_PARENT(pArg)
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_eType = CG_MONSTER;

	return S_OK;
}

void CHarpoonguy::Priority_Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
	{
		CGameObject* pTarget = m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
		if (nullptr == pTarget)
			return;

		SetTarget(pTarget);
		Safe_AddRef(pTarget);

	}

	if (m_iCurrentFrame > 26)
	{
		m_bIsActive = false;

	}
}

void CHarpoonguy::Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;

	Select_Pattern(fTimeDelta);

	__super::Update(fTimeDelta);


	if (m_eCurState != MS_DEATH)
	{
		m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pColliderCom->Get_Scale());

		m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
	}

}

void CHarpoonguy::Late_Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;



	Select_Frame(fTimeDelta);

	//m_pGameInstance->Add_RenderGroup(CRenderer::RG_COLLIDER, this); 
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CHarpoonguy::Render()
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

void CHarpoonguy::Deserialize(const json& j)
{
	SET_TRANSFORM(j, m_pTransformCom);
}

HRESULT CHarpoonguy::On_Collision(CCollisionObject* other)
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


	return S_OK;
}

void CHarpoonguy::Select_Pattern(_float fTimeDelta)
{
	if (m_eCurState == MS_DEATH)
		return;

	

	_float3 vDist;
	vDist = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION);

	//Shooting(fTimeDelta);

	//거리로 판단해서 패턴 실행하도록 
	if (vDist.LengthSq() > 10)
		Chasing(fTimeDelta);
	else
	{
		Shooting(fTimeDelta);
	}
		
}

void CHarpoonguy::Chasing(_float fTimeDelta)
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

void CHarpoonguy::Shooting(_float fTimeDelta)
{
	if (m_eCurState != MS_ATTACK)
	{
		if (m_fElapsedTime >= 1.f)
			m_eCurState = MS_ATTACK;
		else
			return;
	}

	if (m_iCurrentFrame == 12)
	{

		CProjectile_Base::PROJ_DESC pDesc = {};
		pDesc.fSpeed = 1.f;
		pDesc.vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK).GetNormalized();
		pDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		//CHarpoon* pProj =  static_cast<CHarpoon*>(m_pGameInstance->Add_GameObject_FromPool(LEVEL_GAMEPLAY, LEVEL_GAMEPLAY, TEXT("Layer_Monster_Projectile_Harpoon"), &pDesc));

		m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Harpoon"), LEVEL_GAMEPLAY, TEXT("Layer_Monster_Projectile_Harpoon"), &pDesc);
		
		//m_pGameInstance->Acquire_Object(LEVEL_GAMEPLAY, TEXT("Layer_Monster_Projectile_Harpoon"), &pDes);

	

		m_iCurrentFrame++;
	}


	//m_pGameInstance->Add_GameObject();
}

void CHarpoonguy::Select_Frame(_float fTimeDelta)
{
	m_fElapsedTime += fTimeDelta;



	switch (m_eCurState)
	{
	case MS_IDLE:
		m_iCurrentFrame = 0;
		break;
	case MS_WALK:
		if (m_iCurrentFrame < 2 || m_iCurrentFrame > 7)
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

		if (m_iCurrentFrame < 8 || m_iCurrentFrame > 17)
			m_iCurrentFrame = 8;

		if (m_fElapsedTime >= 0.1f)
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

HRESULT CHarpoonguy::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0); // 기준값 설정 (0~255)

	return S_OK;
}

HRESULT CHarpoonguy::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CHarpoonguy::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_tObjDesc.iLevel, m_tObjDesc.stProtTextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CHarpoonguy* CHarpoonguy::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHarpoonguy* pInstance = new CHarpoonguy(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHarpoonguy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHarpoonguy::Clone(void* pArg)
{
	CHarpoonguy* pInstance = new CHarpoonguy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CHarpoonguy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHarpoonguy::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTarget);
}

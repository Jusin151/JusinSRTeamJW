#include "Looker.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Collider_Cube.h"
#include "Transform.h"
#include "Player.h"
#include "GameInstance.h"
#include "Projectile_Base.h"
#include "Stains_Effect.h"

CLooker::CLooker(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CMonster_Base(pGraphic_Device)
{
}

CLooker::CLooker(const CLooker& Prototype)
    :CMonster_Base(Prototype)
{
}

HRESULT CLooker::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLooker::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_eType = CG_MONSTER;

    m_iHp = 60;

    m_fSpeed = 0.5f;

    m_iExp = 27;

    m_pColliderCom->Set_Scale(_float3(1.f, 1.f, 1.f));

	// 디버깅 용
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(-15.f, 0.46f, -32.f));

    return S_OK;
}

void CLooker::Priority_Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
	{
		CGameObject* pTarget = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"));
		if (nullptr == pTarget)
			return;

		SetTarget(pTarget);
		Safe_AddRef(pTarget);

	}

	if (m_iHp <= 0)
	{

		m_eCurState = MS_DEATH;
		if (!bAdd_Exp)
		{
			if (CPlayer* pTarget = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"))))
			{
				pTarget->Add_Exp(m_iExp);
			}
			bAdd_Exp = true;
		}

	}

	m_vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_vNextPos = m_vCurPos;

	if (m_eCurState == MS_DEATH && !m_bGib)
	{
		m_bGib = true;
		Create_Gibs(0);
		m_pSoundCom->Play_Event(L"event:/Monsters/Looker/Looker_Death", m_pTransformCom)->SetVolume(0.5f);
	}
	if (m_iCurrentFrame >= 42)
	{
		m_bIsActive = false;

	}
}

void CLooker::Update(_float fTimeDelta)
{
	if (m_pTarget == nullptr)
		return;


	Select_Pattern(fTimeDelta);

	__super::Update(fTimeDelta);

	if (m_eCurState != MS_DEATH)
	{
		m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pTransformCom->Compute_Scaled());

		m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
	}

	m_pSoundCom->Update(fTimeDelta);
}

void CLooker::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsAABBInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Compute_Scaled()))
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
			return;
	}

	if (nullptr == m_pTarget)
		return;

	if (m_bHit)
	{
		m_pColliderCom->Set_Scale(_float3(2.5f, 2.5f, 2.5f));
	}

	
	Calc_Position();


	if (m_eCurState != MS_DEATH)
	{
		_float3 fPos = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

		m_vCurPos.y = fPos.y + 0.2f;
		m_vNextPos.y = fPos.y + 0.2f;
	}


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vNextPos);

	_float3 vTargetDist;
	vTargetDist = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION);

	_float3 vDist = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vTargetDist;

	if (vDist.LengthSq() < 2)
	{
		m_vNextPos = vDist.GetNormalized() * 1.5f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vNextPos);
	}

	m_vObjectMtvSum = { 0.f, 0.f, 0.f };
	m_vWallMtvs.clear();

	Select_Frame(fTimeDelta);
}

HRESULT CLooker::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	SetUp_RenderState();
	if (FAILED(m_pShaderCom->Bind_Transform()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, m_iCurrentFrame)))
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


HRESULT CLooker::On_Collision(CCollisionObject* other)
{
	if (nullptr == m_pColliderCom)
		return E_FAIL;

	if (nullptr == other)
		return S_OK;

	// 안바뀌면 충돌 안일어남
	if (other->Get_Type() == CG_END)
		return S_OK;

	_float3 vMtv = m_pColliderCom->Get_MTV();
	_float3 vMove = { vMtv.x, 0.f, vMtv.z };

	switch (other->Get_Type())
	{
	case CG_PLAYER:
		Take_Damage(other);
		break;

	case CG_WEAPON:
		Create_Stains(5);
		m_pSoundCom->Play_Event(L"event:/Monsters/Looker/Looker_Pain", m_pTransformCom)->SetVolume(0.5f);
		m_eCurState = MS_HIT;
		break;
	case CG_MONSTER:
		m_vObjectMtvSum += vMove * 0.5f;
		break;
	case CG_STRUCTURE_WALL:

		break;
	case CG_OBJECT:
		m_vObjectMtvSum += vMove;
		break;
	case CG_DOOR:

		break;
	default:
		break;
	}


	return S_OK;
}

void CLooker::Select_Pattern(_float fTimeDelta)
{
	if (m_eCurState == MS_DEATH)
		return;

	_float3 vDist;
	vDist = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION);


	//Shooting(fTimeDelta);

	switch (m_eCurState)
	{
	case MS_IDLE:
		if (vDist.LengthSq() > 25.f)
		{
		
			Chasing(fTimeDelta, 5.f);
		}
		else
		{
			m_pSoundCom->Play_Event(L"event:/Monsters/Looker/Looker_Detect", m_pTransformCom)->SetVolume(0.5f);
			m_eCurState = MS_ATTACK;
		}
		break;
	case MS_HIT:
		if (m_fElapsedTime >= 0.5f)
			m_eCurState = MS_IDLE;
		else
			return;

		break;
	case MS_ATTACK:
		//
		Shooting(fTimeDelta);
		break;

	default:
		break;
	}
}

void CLooker::Shooting(_float fTimeDelta)
{
	if (m_eCurState != MS_ATTACK)
	{
		if (m_fElapsedTime >= 0.5f)
			m_eCurState = MS_ATTACK;
		else
			return;
	}

	if (m_iCurrentFrame == 22)
	{
		m_pSoundCom->Play_Event(L"event:/Monsters/Looker/Looker_Attack", m_pTransformCom)->SetVolume(0.5f);
		CProjectile_Base::PROJ_DESC pDesc = {};
		pDesc.fSpeed = 8.f;
		pDesc.vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK).GetNormalized();
		pDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		// 오브젝트 풀링으로 변경 필요
		m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_LookerBullet"), m_pGameInstance->Get_CurrentLevel(), 
			TEXT("Layer_Projectile_LookerBullet"), &pDesc);

		m_iCurrentFrame++;
	}
}

void CLooker::Select_Frame(_float fTimeDelta)
{
	if (m_iCurrentFrame == 42)
		return;

	m_fElapsedTime += fTimeDelta ; //임의로 애니메이션 속도 좀 올림

	switch (m_eCurState)
	{
	
	case MS_IDLE:

		if (m_iCurrentFrame == 9)
		{
			m_iCurrentFrame = 0;
			return;
		}

		if (m_iCurrentFrame < 0 || m_iCurrentFrame > 9)
			m_iCurrentFrame = 0;

		if (m_fElapsedTime >= 0.2f)
		{
			m_fElapsedTime = 0.0f;

			m_iCurrentFrame++;

		}
		break;
	case MS_HIT:
		m_iCurrentFrame = 10;
		break;
	case MS_ATTACK:

		if (m_iCurrentFrame == 27)
		{
			m_eCurState = MS_IDLE;
			m_iCurrentFrame = 0;
			return;
		}

		if (m_iCurrentFrame < 11 || m_iCurrentFrame > 27)
			m_iCurrentFrame = 11;

		if (m_fElapsedTime >= 0.1f)
		{
			m_fElapsedTime = 0.0f;

			m_iCurrentFrame++;

		}
		break;
	case MS_DEATH:
		if (m_iCurrentFrame < 28)
			m_iCurrentFrame = 28;

		if (m_fElapsedTime >= 0.07f)
		{
			m_fElapsedTime = 0.0f;

			m_iCurrentFrame++;

		}
		break;

	default:
		break;
	}
}

HRESULT CLooker::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0); // 기준값 설정 (0~255)
	_float2 ScaleFactor = { 1.0f, 1.0f };
	_float2 Offset = { 0.f, 0.f };
	m_pShaderCom->Set_UVScaleFactor(&ScaleFactor);
	m_pShaderCom->Set_UVOffsetFactor(&Offset);
	return S_OK;
}

HRESULT CLooker::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}


HRESULT CLooker::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Looker"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	return S_OK;
}

CLooker* CLooker::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLooker* pInstance = new CLooker(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLooker");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLooker::Clone(void* pArg)
{
	CLooker* pInstance = new CLooker(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CLooker");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLooker::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTarget);
}

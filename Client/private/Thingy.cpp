#include "Thingy.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Collider_Cube.h"
#include "Player.h"
#include "GameInstance.h"
#include "Stains_Effect.h"
#include "Projectile_Base.h"

CThingy::CThingy(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CMonster_Base(pGraphic_Device)
{
}

CThingy::CThingy(const CThingy& Prototype)
	:CMonster_Base(Prototype)
{
}

HRESULT CThingy::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CThingy::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_eType = CG_MONSTER;

	m_iAp = 10;

	m_iHp = 60;

	m_iExp = 64;

	m_fSpeed = 0.4f;


	m_pColliderCom->Set_Scale(_float3(2.5f, 2.5f, 2.5f));

	m_pTransformCom->Set_Scale(2.5f, 2.5f, 2.5f);

	// 디버깅 용
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(-15.f, 0.46f, -32.f));

	// morph 상태로 소환
	m_eCurState = MS_MORPH;

	m_iCurrentFrame = 2;

	return S_OK;
}

void CThingy::Priority_Update(_float fTimeDelta)
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
		m_pSoundCom->Play_Event(L"event:/Monsters/Polarman/Polarman_Death", m_pTransformCom)->SetVolume(0.5f);
	}
	if (m_iCurrentFrame >= 77)
	{
		m_bIsActive = false;

	}
}

void CThingy::Update(_float fTimeDelta)
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


void CThingy::Late_Update(_float fTimeDelta)
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

	if(m_eCurState != MS_ATTACK_MELEE)
		Calc_Position();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vNextPos);

	m_vObjectMtvSum = { 0.f, 0.f, 0.f };
	m_vWallMtvs.clear();

	Select_Frame(fTimeDelta);
}

HRESULT CThingy::Render()
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

HRESULT CThingy::On_Collision(CCollisionObject* other)
{
	if (nullptr == m_pColliderCom)
		return E_FAIL;

	if (nullptr == other)
		return S_OK;

	// 안바뀌면 충돌 안일어남
	if (other->Get_Type() == CG_END)
		return S_OK;

	_float3 vMtv = m_pColliderCom->Get_MTV();
	_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vMove = { vMtv.x, 0.f, vMtv.z };

	switch (other->Get_Type())
	{
	case CG_PLAYER:
		if (m_eCurState == MS_ATTACK_MELEE && m_iCurrentFrame >= 29)
		{
			m_bHit = true;
			Take_Damage(other);
		}
			
		break;

	case CG_WEAPON:
		Create_Stains(5);
		m_pSoundCom->Play_Event(L"event:/Monsters/Polarman/Polarman_Pain", m_pTransformCom)->SetVolume(0.5f);
		if(m_eCurState == MS_IDLE || m_eCurState == MS_WALK)
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

void CThingy::Select_Pattern(_float fTimeDelta)
{
	if (m_eCurState == MS_DEATH)
		return;

	if (m_eCurState == MS_MORPH)
		return;

	_float3 vDist;
	vDist = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION);

	switch (m_eCurState)
	{
	case MS_IDLE:
		if (vDist.LengthSq() > 30)
		{
			m_eCurState = MS_WALK;
		}
		else
		{
			if (vDist.Length() > m_pColliderCom->Get_Scale().Length())
				m_eCurState = MS_ATTACK_RANGE;
			else
				m_eCurState = MS_ATTACK_MELEE;
		}
		break;
	case MS_WALK:
		m_pSoundCom->Play_Event(L"event:/Monsters/Polarman/Polarman_Detect", m_pTransformCom)->SetVolume(0.5f);
		Chasing(fTimeDelta, m_pColliderCom->Get_Scale().Length());
		break;
	case MS_HIT:
		if (m_fElapsedTime >= 0.5f)
			m_eCurState = MS_IDLE;
		else
			return;

		break;
	case MS_ATTACK_RANGE:
		m_pSoundCom->Play_Event(L"event:/Monsters/Polarman/Polarman_Attack", m_pTransformCom)->SetVolume(0.5f);
		Shooting(fTimeDelta);
		break;

	case MS_ATTACK_MELEE:
		m_pSoundCom->Play_Event(L"event:/Monsters/Polarman/Polarman_Attack", m_pTransformCom)->SetVolume(0.5f);
		Melee_Attack(fTimeDelta);
		break;

	default:
		break;
	}

}

void CThingy::Shooting(_float fTimeDelta)
{
	if (m_eCurState != MS_ATTACK_RANGE)
	{
		if (m_fElapsedTime >= 0.5f)
			m_eCurState = MS_ATTACK_RANGE;
		else
			return;
	}

	if (m_iCurrentFrame == 48)
	{

		CProjectile_Base::PROJ_DESC pDesc = {};
		
	
		pDesc.vPos = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

		pDesc.vPos += static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_LOOK).GetNormalized();

		// 오브젝트 풀링으로 변경 필요
		m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_ThingySpike"), m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Projectile_ThingSpike"), &pDesc);
		m_iCurrentFrame++;
	}
}

void CThingy::Melee_Attack(_float fTimeDelta)
{
	if (m_eCurState != MS_ATTACK_MELEE)
	{
		if (m_fElapsedTime >= 0.5f)
			m_eCurState = MS_ATTACK_MELEE;
		else
			return;
	}

	if(!m_bHit)
		m_pColliderCom->Set_Scale(_float3(3.f, 3.f, 3.f));

	// 29 
}

void CThingy::Select_Frame(_float fTimeDelta)
{

	if (m_iCurrentFrame == 77)
		return;
		
	m_fElapsedTime += fTimeDelta;



	switch (m_eCurState)
	{
	case Client::CMonster_Base::MS_IDLE:
		m_iCurrentFrame = 0;
		break;
	case Client::CMonster_Base::MS_HIT:
		m_iCurrentFrame = 1;
		break;
	case Client::CMonster_Base::MS_WALK:
		if (m_iCurrentFrame == 25)
		{
			m_eCurState = MS_IDLE;
			m_iCurrentFrame = 0;
			return;
		}

		if (m_iCurrentFrame < 20|| m_iCurrentFrame > 25)
			m_iCurrentFrame = 20;

		if (m_fElapsedTime >= 0.2f)
		{
			m_fElapsedTime = 0.0f;

			m_iCurrentFrame++;

		}
		break;
	case Client::CMonster_Base::MS_ATTACK_MELEE:
		if (m_iCurrentFrame == 39)
		{
			m_eCurState = MS_IDLE;
			m_iCurrentFrame = 0;
			return;
		}

		if (m_iCurrentFrame < 26 || m_iCurrentFrame > 39)
			m_iCurrentFrame = 26;

		if (m_fElapsedTime >= 0.07f)
		{
			m_fElapsedTime = 0.0f;

			m_iCurrentFrame++;

		}
		break;
	case Client::CMonster_Base::MS_ATTACK_RANGE:
		if (m_iCurrentFrame == 67)
		{
			m_eCurState = MS_IDLE;
			m_iCurrentFrame = 0;
			return;
		}

		if (m_iCurrentFrame < 40 || m_iCurrentFrame > 67)
			m_iCurrentFrame = 40;

		if (m_fElapsedTime >= 0.07f)
		{
			m_fElapsedTime = 0.0f;

			m_iCurrentFrame++;

		}
		break;
	case Client::CMonster_Base::MS_MORPH:
		if (m_iCurrentFrame == 19)
		{
			m_eCurState = MS_IDLE;
			m_iCurrentFrame = 0;
			return;
		}

		if (m_iCurrentFrame < 2 || m_iCurrentFrame > 19)
			m_iCurrentFrame = 2;

		if (m_fElapsedTime >= 0.07f)
		{
			m_fElapsedTime = 0.0f;

			m_iCurrentFrame++;

		}
		break;
	case Client::CMonster_Base::MS_DEATH:
		if (m_iCurrentFrame < 68)
			m_iCurrentFrame = 68;

		if (m_fElapsedTime >= 0.1f)
		{
			m_fElapsedTime = 0.0f;

			m_iCurrentFrame++;

		}
		break;
	default:
		break;
	}
}

HRESULT CThingy::SetUp_RenderState()
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

HRESULT CThingy::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CThingy::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC,TEXT("Prototype_Component_Texture_Thingy"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	return S_OK;
}

CThingy* CThingy::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CThingy* pInstance = new CThingy(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CThingy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CThingy::Clone(void* pArg)
{
	CThingy* pInstance = new CThingy(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CHarpoonguy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CThingy::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTarget);
}

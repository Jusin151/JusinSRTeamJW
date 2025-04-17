#include "Hellhound.h"
#include "Texture.h"
#include "Collider_Cube.h"
#include "Player.h"
#include "GameInstance.h"
#include "Stains_Effect.h"
#include "Gib_Effect.h"
#include "AttackMelee.h"

CHellhound::CHellhound(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CMonster_Base(pGraphic_Device)
{
}

CHellhound::CHellhound(const CHellhound& Prototype)
    :CMonster_Base(Prototype)
{
}

HRESULT CHellhound::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHellhound::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_eType = CG_MONSTER;

	m_iAp = 10;

	m_iHp = 100;

	m_iExp = 64;

	m_fSpeed = 0.7f;


	m_pColliderCom->Set_Scale(_float3(3.f, 3.f, 3.f));

	m_pTransformCom->Set_Scale(3.f, 3.f, 3.f);

	// 디버깅 용
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(-15.f, 0.46f, -32.f));

	// morph 상태로 소환
	m_eCurState = MS_IDLE;

	m_iCurrentFrame = 0;

	return S_OK;
}

void CHellhound::Priority_Update(_float fTimeDelta)
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
		Create_Gibs(6);
		m_pSoundCom->Play_Event(L"event:/Monsters/Hellhound/Hellhound_Death", m_pTransformCom)->SetVolume(0.5f);
	}
	if (m_iCurrentFrame >= 45)
	{
		m_bIsActive = false;

	}
}

void CHellhound::Update(_float fTimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	Check_Hp();

	Select_Pattern(fTimeDelta);

	__super::Update(fTimeDelta);

	if (m_eCurState != MS_DEATH)
	{
		m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pTransformCom->Compute_Scaled());

		m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
	}

	m_pSoundCom->Update(fTimeDelta);
}

void CHellhound::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsAABBInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Compute_Scaled()))
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
			return;
	}

	if (nullptr == m_pTarget)
		return;


	if (m_eCurState != MS_ATTACK)
		Calc_Position();

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

HRESULT CHellhound::Render()
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

HRESULT CHellhound::On_Collision(CCollisionObject* other)
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
		if (m_eCurState == MS_ATTACK)
		{
			
			Take_Damage(other);
			
		}

		break;

	case CG_WEAPON:
		Create_Stains(5);
		m_pSoundCom->Play_Event(L"event:/Monsters/Hellhound/Hellhound_Pain", m_pTransformCom)->SetVolume(0.5f);
		if (m_eCurState == MS_IDLE || m_eCurState == MS_WALK)
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

void CHellhound::Select_Pattern(_float fTimeDelta)
{
	if (m_eCurState == MS_DEATH)
		return;

	if (m_eCurState == MS_MORPH)
		return;

	_float3 vDist;
	vDist = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION);

	vDist.y = 0;

	_float3 fScale = m_pColliderCom->Get_Scale();
	fScale.y = 0;

	switch (m_eCurState)
	{
	case MS_IDLE:
		if (vDist.LengthSq() > fScale.LengthSq())
		{
			m_eCurState = MS_WALK;
		}
		else
		{
			m_eCurState = MS_ATTACK;
		}
		break;

	case MS_WALK:
		m_pSoundCom->Play_Event(L"event:/Monsters/Hellhound/Hellhound_Detect", m_pTransformCom)->SetVolume(0.5f);
		Chasing(fTimeDelta, 4.f);
		break;

	case MS_HIT:
		if (m_fElapsedTime >= 0.5f)
			m_eCurState = MS_IDLE;
		else
			return;
		break;

	case MS_ATTACK:
		m_pSoundCom->Play_Event(L"event:/Monsters/Hellhound/Hellhound_Attack", m_pTransformCom)->SetVolume(0.5f);
		Melee_Attack(fTimeDelta);
		break;

	default:
		break;
	}
}

void CHellhound::Check_Hp()
{
	if (m_bDamaged)
		return;

	if (m_iHp <= 50)
	{
		m_bDamaged = true;
		m_eCurState = MS_MORPH;
	}
	
}

void CHellhound::Melee_Attack(_float fTimeDelta)
{
	if (m_eCurState != MS_ATTACK)
	{
		if (m_fElapsedTime >= 0.5f)
			m_eCurState = MS_ATTACK;
		else
			return;
	}


	if (m_iCurrentFrame == 17 || m_iCurrentFrame == 26)
	{
		CAttackMelee::MELEE_DESC pDesc = {};
		pDesc.iDamage = m_iAp;
		pDesc.fLifeTIme = 0.5f;
		pDesc.vScale = { 3.f, 3.f, 3.f };
		pDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK).GetNormalized();

		m_pGameInstance->Add_GameObject(LEVEL_STATIC, TEXT("Prototype_GameObject_AttackMelee"), m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_AttackMelee"), &pDesc);
	}
}

void CHellhound::Select_Frame(_float fTimeDelta)
{
	if (m_iCurrentFrame == 77)
		return;

	m_fElapsedTime += fTimeDelta;



	switch (m_eCurState)
	{
	case Client::CMonster_Base::MS_IDLE:
		if (m_bDamaged)
		{
			m_iCurrentFrame = 0;
		}
		else
		{
			m_iCurrentFrame = 1;
		}
		
		break;
	case Client::CMonster_Base::MS_HIT:
		if (m_bDamaged)
			m_iCurrentFrame = 30;
		break;
	case Client::CMonster_Base::MS_WALK:
		if (m_bDamaged)
		{
			if (m_iCurrentFrame == 12)
			{
				m_eCurState = MS_IDLE;
				m_iCurrentFrame = 1;
				return;
			}

			if (m_iCurrentFrame < 7 || m_iCurrentFrame > 12)
				m_iCurrentFrame = 7;

			if (m_fElapsedTime >= 0.07f)
			{
				m_fElapsedTime = 0.0f;

				m_iCurrentFrame++;

			}
		}
		else
		{
			if (m_iCurrentFrame == 6)
			{
				m_eCurState = MS_IDLE;
				m_iCurrentFrame = 0;
				return;
			}

			if (m_iCurrentFrame < 2 || m_iCurrentFrame > 6)
				m_iCurrentFrame = 2;

			if (m_fElapsedTime >= 0.07f)
			{
				m_fElapsedTime = 0.0f;

				m_iCurrentFrame++;

			}
		}
		break;
	case Client::CMonster_Base::MS_ATTACK:

		if (m_bDamaged)
		{
			if (m_iCurrentFrame == 29)
			{
				m_eCurState = MS_IDLE;
				m_iCurrentFrame = 1;
				return;
			}

			if (m_iCurrentFrame < 23 || m_iCurrentFrame > 29)
				m_iCurrentFrame = 23;

			if (m_fElapsedTime >= 0.1f)
			{
				m_fElapsedTime = 0.0f;

				m_iCurrentFrame++;

			}
		}
		else
		{
			if (m_iCurrentFrame == 22)
			{
				m_eCurState = MS_IDLE;
				m_iCurrentFrame = 0;
				return;
			}

			if (m_iCurrentFrame < 13 || m_iCurrentFrame > 22)
				m_iCurrentFrame = 13;

			if (m_fElapsedTime >= 0.1f)
			{
				m_fElapsedTime = 0.0f;

				m_iCurrentFrame++;

			}
		}
		break;
	case Client::CMonster_Base::MS_MORPH:
		if (m_iCurrentFrame == 33)
		{
			m_eCurState = MS_IDLE;
			m_iCurrentFrame = 1;
			return;
		}

		if (m_iCurrentFrame == 30)
		{
			if (!m_bArmDestoryed)
			{
				// 팔 gib 생성
				Create_Gibs(5);
				m_bArmDestoryed = true;
			}
		}

		if (m_iCurrentFrame < 30 || m_iCurrentFrame > 33)
			m_iCurrentFrame = 30;

		if (m_fElapsedTime >= 0.3f)
		{
			m_fElapsedTime = 0.0f;

			m_iCurrentFrame++;

		}
	
		break;
	case Client::CMonster_Base::MS_DEATH:
		if (m_iCurrentFrame < 34)
			m_iCurrentFrame = 34;

		if (m_fElapsedTime >= 0.1f)
		{
			m_fElapsedTime = 0.0f;

			m_iCurrentFrame++;

		}
		break;
		break;
	default:
		break;
	}
}

HRESULT CHellhound::SetUp_RenderState()
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

HRESULT CHellhound::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CHellhound::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hellhound"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	

	return S_OK;
}

CHellhound* CHellhound::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHellhound* pInstance = new CHellhound(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHellhound");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHellhound::Clone(void* pArg)
{
	CHellhound* pInstance = new CHellhound(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CHellhound");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellhound::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTarget);

}

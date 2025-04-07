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

	m_iHp = 30;

	m_fSpeed = 0.25f;

	m_pColliderCom->Set_Scale(_float3(1.5f, 1.5f, 1.5f));

	return S_OK;
}

void CHarpoonguy::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (m_iCurrentFrame >= 26)
	{
		m_bIsActive = false;

	}
}

void CHarpoonguy::Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;

	if (!m_bCheck)
	{
		m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pTransformCom->Compute_Scaled());
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

void CHarpoonguy::Late_Update(_float fTimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	if (!m_bCheck)
		return;

	for (auto& wallMTV : m_vWallMtvs)
	{
		auto wallNormal = wallMTV.GetNormalized();
		_float penetration = wallNormal.Dot(m_vObjectMtvSum);
		if (penetration < 0.f)
		{
			// 벽 방향으로 침범 중 → 해당 방향 성분 제거
			m_vObjectMtvSum -= wallNormal * penetration;
		}
	}

	// MTV 크기 클램프 (너무 밀리지 않도록)
	const _float maxMtvLength = 1.5f;
	_float mtvLength = m_vObjectMtvSum.Length();
	if (mtvLength > maxMtvLength)
		m_vObjectMtvSum = m_vObjectMtvSum.GetNormalized() * maxMtvLength;

	if (m_vWallMtvs.empty())
	{
		// 벽 충돌 
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vNextPos + m_vObjectMtvSum);
	}
	else
	{
		if (mtvLength > 0.001f)
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vCurPos);
		else
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_vObjectMtvSum);
	}



	_float3 vScale = m_pTransformCom->Compute_Scaled();
	_float3 extents = _float3(
		0.5f * vScale.x,
		0.5f * vScale.y,
		0.5f * vScale.z
	);
	if (m_pGameInstance->IsPointInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
			return;
	}
	Select_Frame(fTimeDelta);
	

	//m_pGameInstance->Add_RenderGroup(CRenderer::RG_COLLIDER, this); 
	
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

	if (g_bDebugCollider)
	{
 		m_pColliderCom->Render();
	}


	//m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("활잽이 체력:") + to_wstring(m_iHp),
	//	_float2(400.f, -110.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

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

	_float3 vMtv = m_pColliderCom->Get_MTV();
	_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vMove = { vMtv.x, 0.f, vMtv.z };

	switch (other->Get_Type())
	{
	case CG_PLAYER:

		
		Take_Damage(other);
		m_vObjectMtvSum += vMove;
		break;

	case CG_WEAPON:
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

void CHarpoonguy::Select_Pattern(_float fTimeDelta)
{
	if (m_eCurState == MS_DEATH)
		return;

	_float3 vDist;
	vDist = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION);

	
	//Shooting(fTimeDelta);

	switch (m_eCurState)
	{
	case MS_IDLE:
		if (vDist.LengthSq() > 90)
		{
			
			m_eCurState = MS_WALK;
			
		}
		else
		{
			m_eCurState = MS_ATTACK;
		}
		break;
	case MS_WALK:
		Chasing(fTimeDelta);
		break;
	case MS_HIT:
		if (m_fElapsedTime >= 0.5f)
			m_eCurState = MS_IDLE;
		else
			return;

		break;
	case MS_ATTACK:
		Shooting(fTimeDelta);
		break;

	default:
		break;
	}

	
		
}


void CHarpoonguy::Shooting(_float fTimeDelta)
{
	if (m_eCurState != MS_ATTACK)
	{
		if (m_fElapsedTime >= 0.5f)
			m_eCurState = MS_ATTACK;
		else
			return;
	}

	if (m_iCurrentFrame == 12)
	{

		CProjectile_Base::PROJ_DESC pDesc = {};
		pDesc.fSpeed = 8.f;
		pDesc.vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK).GetNormalized();
		pDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		// 오브젝트 풀링으로 변경 필요
		m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Harpoon"), LEVEL_GAMEPLAY, TEXT("Layer_Monster_Projectile_Harpoon"), &pDesc);

		m_iCurrentFrame++;
	}


	//m_pGameInstance->Add_GameObject();
}

void CHarpoonguy::Select_Frame(_float fTimeDelta)
{

	if (m_iCurrentFrame == 26)
		return;

	m_fElapsedTime += fTimeDelta * 1.7f; //임의로 애니메이션 속도 좀 올림

	switch (m_eCurState)
	{
	case MS_IDLE:
		m_iCurrentFrame = 0;
		break;
	case MS_WALK:
		if (m_iCurrentFrame == 7)
		{
			m_eCurState = MS_IDLE;
			m_iCurrentFrame = 0;
			return;
		}

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
			m_iCurrentFrame = 0;
			return;
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

		if (m_fElapsedTime >= 0.15f)
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
	if (FAILED(__super::Add_Component(m_tObjDesc.iProtoLevel, m_tObjDesc.stProtTextureTag,
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

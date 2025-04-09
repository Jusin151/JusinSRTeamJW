#include "Spike.h"
#include <Player.h>

CSpike::CSpike(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CMonster_Base(pGraphic_Device)
{
}

CSpike::CSpike(const CSpike& Prototype)
	:CMonster_Base(Prototype)
{
}

HRESULT CSpike::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpike::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_iHp = 100;
	m_pTarget = m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_Player");
	m_pTransformCom->Set_Scale(5.f, 5.f, 2.f);
	m_fFrame = 0.f;
	Init_Textures();
	return S_OK;
}

void CSpike::Priority_Update(_float fTimeDelta)
{
}

void CSpike::Update(_float fTimeDelta)
{
	if (m_pColliderCom )
	{
		m_pColliderCom->Set_WorldMat(m_pTransformCom->Get_WorldMat());

		m_pColliderCom->Update_Collider(TEXT("Com_Collider_Cube"), m_pTransformCom->Compute_Scaled());
		//m_pGameInstance->Add_Collider(CG_ENVIRONMENT, m_pColliderCom);
		
			m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
	}

	Billboarding(fTimeDelta);
	if (m_bUpdateAnimation)
	{
		Update_Animation(fTimeDelta);
	}
}

void CSpike::Late_Update(_float fTimeDelta)
{
	_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	fPos.y = m_fOffset;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
	if (m_pGameInstance->IsAABBInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Compute_Scaled()))
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
	}
}

HRESULT CSpike::Render()
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

void CSpike::Reset()
{
	m_eState = SPIKE_STATE::ATTACK;
	m_bUpdateAnimation = true;
}

HRESULT CSpike::On_Collision(CCollisionObject* other)
{
	return E_NOTIMPL;
}

void CSpike::Billboarding(_float fTimeDelta)
{
	if (!m_pTarget) return;


	CTransform* pPlayerTransform = static_cast<CPlayer*>(m_pTarget)->Get_TransForm();

	_float3 vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

	vPlayerLook.y = 0.f;
	vPlayerLook.Normalize();

	_float3 vLook = -vPlayerLook;


	_float3 vUp = _float3(0.0f, 1.0f, 0.0f);
	_float3 vRight = vUp.Cross(vLook);
	vRight.Normalize();

	_float3 vNewUp = vLook.Cross(vRight);
	vNewUp.Normalize();

	_float3 vScale = m_pTransformCom->Compute_Scaled();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vNewUp * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * vScale.z);
}

void CSpike::Select_Pattern(_float fTimeDelta)
{
}

void CSpike::Set_Position(const _float3& vPos)
{
}

_bool CSpike::IsAnimationFinished() const
{
	return _bool();
}

HRESULT CSpike::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 240);
	return S_OK;
}

HRESULT CSpike::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CSpike::Ready_Components()
{
	// 컴포넌트 초기화 코드
	if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Spike"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;

	ColliderDesc.fScale = { 1.f, 1.f, 3.f };
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };
	m_eType = CG_MONSTER;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

void CSpike::Init_Textures()
{
	for (_uint i = 0; i < m_pTextureCom->Get_NumTextures(); i++)
	{
		if (i < 5)
		{
			m_mapStateTextures[SPIKE_STATE::ATTACK].push_back(i);
		}
		else
		{
			m_mapStateTextures[SPIKE_STATE::DISAPPEAR].push_back(i);
		}
	}
}

void CSpike::Update_Animation(_float fTimeDelta)
{
	m_fFrame += m_fAnimationSpeed * fTimeDelta;
	size_t numFrames = m_mapStateTextures[m_eState].size();

	if (m_fFrame >= numFrames)
	{
		m_bAnimFinished = true;
		if (m_eState == SPIKE_STATE::ATTACK)
		{
			// APPEAR 애니메이션이 끝나면 공격 상태로 전환
			m_eState = SPIKE_STATE::DISAPPEAR;
			m_fFrame = 0.f;
			m_iCurrentFrame = m_mapStateTextures[m_eState][0];
		}
		else if (m_eState == SPIKE_STATE::DISAPPEAR)
		{
			m_eState = SPIKE_STATE::ATTACK;
			m_fFrame = 0.f;
			m_iCurrentFrame = m_mapStateTextures[m_eState][0];
		//	SetActive(false);
		}
	}
	else
	{
		m_bAnimFinished = false;
		m_iCurrentFrame = m_mapStateTextures[m_eState][(_uint)m_fFrame];
	}
}

void CSpike::Attack()
{
}

CSpike* CSpike::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSpike* pInstance = new CSpike(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to create CSpike instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSpike::Clone(void* pArg)
{
	CSpike* pClone = new CSpike(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("Failed to clone CSpike instance");
		Safe_Release(pClone);
	}
	return pClone;
}

void CSpike::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
}

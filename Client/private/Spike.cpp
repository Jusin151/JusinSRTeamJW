#include "Spike.h"
#include <Player.h>
#include "WarningZone.h"
#include "Camera_FirstPerson.h"

CSpike::CSpike(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CMonster_Base(pGraphic_Device)
{
}

CSpike::CSpike(const CSpike& Prototype)
	:CMonster_Base(Prototype),
	m_pCamera(Prototype.m_pCamera),
	m_pCameraTransform(Prototype.m_pCameraTransform)
{
	m_pTransformCom = nullptr;
}

HRESULT CSpike::Initialize_Prototype()
{
  	m_pCamera = static_cast<CCamera_FirstPerson*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Camera")));
	if (m_pCamera)
	{
		m_pCameraTransform = static_cast<CTransform*>(m_pCamera->Get_Component(TEXT("Com_Transform")));
	}
	auto pWaring = CWarningZone::Create(m_pGraphic_Device);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_GameObject_WaringZone"), pWaring)))
	{
		Safe_Release(pWaring);
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_BOSS, TEXT("Prototype_GameObject_WaringZone"), TEXT("Layer_WaringZone"), 54)))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSpike::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_iHp = 100;
	m_pTarget = m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_Player");
	m_pTransformCom->Set_Scale(5.f, 5.f, 5.f);
	m_fFrame = 0.f;
	m_iAp = 2;
	Init_Textures();


	m_pWarningZone = static_cast<CWarningZone*>(m_pGameInstance->Add_GameObject_FromPool(LEVEL_BOSS, LEVEL_BOSS, TEXT("Layer_WaringZone")));
	
	if (m_pWarningZone)
	{
	m_pWarningZoneTransform = static_cast<CTransform*>(m_pWarningZone->Get_Component(TEXT("Com_Transform")));
	}
	m_fWarningElapsedTime = 0.f;

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
		m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
	}

	Billboarding(fTimeDelta);
	if (m_bUpdateAnimation)
	{
		Attack();
		Apply_Shake(fTimeDelta);
		Update_Animation(fTimeDelta);
	}
	else
	{
		if (m_pWarningZone && m_pWarningZone->IsActive())
		{
			m_fWarningElapsedTime += fTimeDelta*1.2f;
			_float fNewScale =1.2f + m_fWarningElapsedTime;  // 예: 1초당 1단위씩 증가
			m_pWarningZoneTransform->Set_Scale(fNewScale, fNewScale, 1.0f);
		}
	}
}

void CSpike::Late_Update(_float fTimeDelta)
{
	_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	fPos.y = m_fOffset;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
	if (m_bUpdateAnimation)
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
	m_pShaderCom->Set_Fog(_float3(0.247f, 0.55f, 0.407f), 0.f, 15.f);
	if (FAILED(m_pShaderCom->Bind_Transform()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Lights()))
		return E_FAIL;
	SetUp_RenderState();
	if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, m_iCurrentFrame)))
		return E_FAIL;
	m_pShaderCom->Begin(0);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	m_pShaderCom->End();
	Release_RenderState();



	return S_OK;
}

void CSpike::Reset()
{
	m_eState = SPIKE_STATE::ATTACK;
	m_bUpdateAnimation = true;
	m_fFrame = 0.f;
	m_iCurrentFrame = m_mapStateTextures[m_eSpikeType][m_eState][0];
	m_bAnimFinished = false;
	m_fWarningElapsedTime = 0.f;
	m_bCanAttack = true;
}

HRESULT CSpike::On_Collision(CCollisionObject* other)
{

	return S_OK;
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
	if (m_bIsRight)
	{
		vScale.x = -abs(vScale.x);
	}
	else
	{
		vScale.x = abs(vScale.x);
	}
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vNewUp * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook * vScale.z);
}

void CSpike::Select_Pattern(_float fTimeDelta)
{
}

void CSpike::Flip(_bool bIsRight)
{
	if (m_eSpikeType == SPIKE_TYPE::CENTER) return;
	m_bIsRight = bIsRight;
	
}

void CSpike::Set_Position(const _float3& vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	_float3 zonePos = vPos;
	zonePos.y = -0.44f;
	m_pWarningZone->Set_Position(zonePos);
}

_bool CSpike::IsAnimationFinished() const
{
	return m_bAnimFinished;
}

void CSpike::Activate_WarningZone(_bool bActive )
{
	if (m_pWarningZone)
	{
		m_pWarningZone->Set_Render(bActive);
	}
}

HRESULT CSpike::SetUp_RenderState()
{
	D3DXVECTOR2 vScaleFactor(1.f, 1.f);
	D3DXVECTOR2 vOffsetFactor(0.0f, 0.0f); // Y축 반전을 위한 오프셋 조정
	m_pShaderCom->Set_UVScaleFactor(&vScaleFactor);
	m_pShaderCom->Set_UVOffsetFactor(&vOffsetFactor);
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

	ColliderDesc.fScale = { 1.f, 1.f, 1.f };
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };
	m_eType = CG_MONSTER;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Material */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BaseShader"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

void CSpike::Init_Textures()
{
	for (_uint i = 0; i < m_pTextureCom->Get_NumTextures(); i++)
	{
		if (i < 5)
		{
			m_mapStateTextures[SPIKE_TYPE::SIDE][SPIKE_STATE::ATTACK].push_back(i);
		}
		else if(i>=5&&i<10)
		{
			m_mapStateTextures[SPIKE_TYPE::SIDE][SPIKE_STATE::DISAPPEAR].push_back(i);
		}
		else if (i >= 10 && i < 15)
		{
			m_mapStateTextures[SPIKE_TYPE::CENTER][SPIKE_STATE::ATTACK].push_back(i);
		}
		else
		{
			m_mapStateTextures[SPIKE_TYPE::CENTER][SPIKE_STATE::DISAPPEAR].push_back(i);
		}
	}
}

void CSpike::Update_Animation(_float fTimeDelta)
{
	m_fFrame += m_fAnimationSpeed * fTimeDelta;
	size_t numFrames = m_mapStateTextures[m_eSpikeType][m_eState].size();

	if (m_fFrame >= numFrames)
	{
		if (m_eState == SPIKE_STATE::ATTACK)
		{
		
			m_eState = SPIKE_STATE::DISAPPEAR;
			m_fFrame = 0.f;
			m_bAnimFinished = true; // 공격이 끝나서 다음 촉수한테 알려주기 위함
			m_iCurrentFrame = m_mapStateTextures[m_eSpikeType][m_eState][0];
		}
		else if (m_eState == SPIKE_STATE::DISAPPEAR)
		{
			if (m_pWarningZone)
			{
				m_pWarningZone->Set_Render(false);
			}
			m_fFrame = 0.f;
			m_iCurrentFrame = m_mapStateTextures[m_eSpikeType][m_eState][0];
			SetActive(false);
		}
	}
	else
	{
		m_iCurrentFrame = m_mapStateTextures[m_eSpikeType][m_eState][(_uint)m_fFrame];
	}
}

void CSpike::Attack()
{
	if (m_eState != SPIKE_STATE::ATTACK||!m_bCanAttack)
	{
		return;
	}

	size_t numFrames = m_mapStateTextures[m_eSpikeType][m_eState].size();

	if ((_uint)(numFrames - 1) == (_uint)m_fFrame)
	{
		_float3 spikePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		spikePos.y = 0.f;
		_float3 playerPos = m_pCameraTransform->Get_State(CTransform::STATE_POSITION);
		playerPos.y = 0.f;
		_float distance = _float3::Distance(playerPos, spikePos);
		if (abs(distance) <= ATTACK_DISTANCE_THRESHOLD)
		{
			if (m_pTarget )
			{
				static_cast<CPlayer*>(m_pTarget)->Take_Damage(10); // 10 데미지 적용 예시
				m_bCanAttack = false;
			}

		}
	}
}

void CSpike::Apply_Shake(_float fTimeDelta)
{
	if (m_bShaked == false) return;
	if (m_pCamera)
	{
		m_pCamera->TriggerShake(0.2f, 0.5f);
	}
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

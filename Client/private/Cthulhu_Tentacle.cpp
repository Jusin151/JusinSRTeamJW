#include "Cthulhu_Tentacle.h"
#include "GameInstance.h"
#include "Collider_Cube.h"
#include "StructureManager.h"
#include <CthulhuMissile.h>
#include <Player.h>
#include "HP_WorldUI.h"
#include <Cthulhu.h>

CCthulhu_Tentacle::CCthulhu_Tentacle(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CMonster_Base(pGraphic_Device)
{
}

CCthulhu_Tentacle::CCthulhu_Tentacle(const CCthulhu_Tentacle& Prototype)
	:CMonster_Base(Prototype),
	m_pOwner(Prototype.m_pOwner)
{
}

HRESULT CCthulhu_Tentacle::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CCthulhu_Tentacle::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_iHp =150;
	m_iMaxHp = m_iHp;
	m_pTarget = m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_Player");
	m_pTransformCom->Set_Scale(2.f, 2.f, 2.f);
	m_fFrame = 0.f;
	Init_Textures();
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_BOSS, TEXT("Prototype_GameObject_World_HpBar"), LEVEL_BOSS, TEXT("Layer_UI_HpBarWorld"))))
	{
		return E_FAIL;
	}

	m_pHpBar = static_cast<CHP_WorldUI*>( m_pGameInstance->Find_Last_Object(LEVEL_BOSS, TEXT("Layer_UI_HpBarWorld")));
	m_pHpBar->Set_Parent(this);
	m_pHpBar->SetActive(false);
	return S_OK;
}

void CCthulhu_Tentacle::Priority_Update(_float fTimeDelta)
{
	if (m_iHp <= 0)
	{
		m_eState = Tentacle_STATE::DEAD;
	}
	Select_Pattern(fTimeDelta);
}

void CCthulhu_Tentacle::Update(_float fTimeDelta)
{
	if(m_pColliderCom && m_eState != Tentacle_STATE::DEAD )
	{
		m_pColliderCom->Set_WorldMat(m_pTransformCom->Get_WorldMat());

		m_pColliderCom->Update_Collider(TEXT("Com_Collider_Cube"),m_pTransformCom->Compute_Scaled());
		m_pGameInstance->Add_Collider(CG_ENVIRONMENT, m_pColliderCom);
		if(m_bCanHit)
		m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
	}

	Billboarding(fTimeDelta);
	if (m_bUpdateAnimation)
	{
		Update_Animation(fTimeDelta);
	}


}

void CCthulhu_Tentacle::Late_Update(_float fTimeDelta)
{
	_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	fPos.y = m_fOffset;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
	if (m_pGameInstance->IsAABBInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Compute_Scaled()))
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
	}
}

HRESULT CCthulhu_Tentacle::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	m_pShaderCom->Set_Fog(_float3(0.247f, 0.55f, 0.407f), 1.f, 35.f);
	if (FAILED(m_pShaderCom->Bind_Transform()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Material(m_pMaterialCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Lights()))
		return E_FAIL;
	SetUp_RenderState();
	if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, m_iCurrentFrame)))
		return E_FAIL;
	m_pShaderCom->Begin(1);

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	m_pShaderCom->End();
	Release_RenderState();
	return S_OK;
}

void CCthulhu_Tentacle::Reset()
{
	m_eState = Tentacle_STATE::APPEAR;
}

void CCthulhu_Tentacle::Set_Hp(_int iHp)
{
	m_iHp = iHp;
	_float fHpRatio =(_float)m_iHp / m_iMaxHp;
	m_pSoundCom->Play_Event(L"event:/Monsters/the/the_thing_death_pain_1", m_pTransformCom)->SetVolume(0.5f);
	m_pHpBar->OnNotify(&fHpRatio, TEXT(""));
}

HRESULT CCthulhu_Tentacle::On_Collision(CCollisionObject* other)
{
    return E_NOTIMPL;
}

void CCthulhu_Tentacle::Billboarding(_float fTimeDelta)
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

void CCthulhu_Tentacle::Select_Pattern(_float fTimeDelta)
{

	switch (m_eState)
	{
	case Client::CCthulhu_Tentacle::Tentacle_STATE::IDLE:
		m_bCanHit = true;
		//if (IsPlayerVisible())
		//{
		//	m_bUpdateAnimation = true;
		//	m_eState = Tentacle_STATE::ATTACK;
		//	m_fFrame = 0.f;
		//	m_iCurrentFrame = m_mapStateTextures[m_eState][(_uint)m_fFrame];
		//}
		m_eState = Tentacle_STATE::ATTACK;

		break;
	case Tentacle_STATE::APPEAR:
	case Tentacle_STATE::DISAPPEAR:
		m_bCanHit = false;
		break;
	case Client::CCthulhu_Tentacle::Tentacle_STATE::ATTACK:
		break;
	
	case Client::CCthulhu_Tentacle::Tentacle_STATE::DEAD:
		if (m_pHpBar)
		{
		m_pHpBar->SetActive(false);
		}
		break;
	default:
		break;
	}
}

void CCthulhu_Tentacle::Set_Position(const _float3& vPos)
{
	if (m_pTransformCom)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

_bool CCthulhu_Tentacle::IsAnimationFinished() const
{
	return m_bAnimFinished;
}

HRESULT CCthulhu_Tentacle::SetUp_RenderState()
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

HRESULT CCthulhu_Tentacle::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CCthulhu_Tentacle::Ready_Components()
{
	// 컴포넌트 초기화 코드
	if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Cthulhu_Tentacle"),
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

	/* For.Com_Material */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), reinterpret_cast<CComponent**>(&m_pMaterialCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_BaseShader"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound_Source"),
		TEXT("Com_Sound_Source"), reinterpret_cast<CComponent**>(&m_pSoundCom))))
		return E_FAIL;
	return S_OK;
}

void CCthulhu_Tentacle::Init_Textures()
{
	for (_uint i = 0; i < m_pTextureCom->Get_NumTextures(); i++)
	{
		if (i == 5)
		{
			m_mapStateTextures[Tentacle_STATE::IDLE].push_back(i);
		}
		else if (i < 5)
		{
			m_mapStateTextures[Tentacle_STATE::APPEAR].push_back(i);
		}
		else if (i > 5 && i < 14)
		{
			m_mapStateTextures[Tentacle_STATE::DISAPPEAR].push_back(i);
		}
		else if (i > 13 && i < 24)
		{
			m_mapStateTextures[Tentacle_STATE::DEAD].push_back(i);
		}
		else if (i > 24)
		{
			m_mapStateTextures[Tentacle_STATE::ATTACK].push_back(i);
		}
	}
}

void CCthulhu_Tentacle::Update_Animation(_float fTimeDelta)
{
	m_fFrame += m_fAnimationSpeed * fTimeDelta;
	size_t numFrames = m_mapStateTextures[m_eState].size();

	if (m_fFrame >= numFrames)
	{
		m_bAnimFinished = true;
		if (m_eState == Tentacle_STATE::APPEAR)
		{
			// APPEAR 애니메이션이 끝나면 공격 상태로 전환
			m_eState = Tentacle_STATE::ATTACK;
			m_fFrame = 0.f;
			m_iCurrentFrame = m_mapStateTextures[m_eState][0];
		}
		else if (m_eState == Tentacle_STATE::DISAPPEAR)
		{
			m_eState = Tentacle_STATE::IDLE;
			m_fFrame = 0.f;
			m_iCurrentFrame = m_mapStateTextures[m_eState][0];
			SetActive(false);
		}
		else if (m_eState == Tentacle_STATE::DEAD)
		{
			SetActive(false);
		}
		else
		{
			m_fFrame = 0.f;
			m_iCurrentFrame = m_mapStateTextures[m_eState][0];
		}
	}
	else
	{
		m_bAnimFinished = false;
		m_iCurrentFrame = m_mapStateTextures[m_eState][(_uint)m_fFrame];

		// 만약 현재 상태가 ATTACK 상태이고 프레임 번호가 31이면 미사일 발사
		if (m_eState == Tentacle_STATE::ATTACK && m_iCurrentFrame == 31)
		{
			if (IsPlayerVisible())
			{
				m_bUpdateAnimation = true;
				Attack();
			}
		}
	}
}

_bool CCthulhu_Tentacle::IsPlayerVisible()
{
	_float3 monsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	monsterPos.y += 5.f+m_fOffset;

	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
	if (!pPlayerTransform)
		return false;
	_float3 playerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_float3 rayOrigin = monsterPos;
	_float3 rayDir = (playerPos - monsterPos);
	rayDir.Normalize();
	_float fDistance = _float3(playerPos - monsterPos).Length();
	auto walls = CStructureManager::Get_Instance()->Get_Structures();

	for (const auto& wall : walls)
	{
		if (wall == nullptr || wall->Get_Tag().find(L"Wall") == wstring::npos)
			continue;
		CCollider_Cube* pWallCollider = dynamic_cast<CCollider_Cube*>(wall->Get_Component(TEXT("Com_Collider_Cube")));

		if (CCthulhu::RayCubeIntersection(rayOrigin, rayDir, pWallCollider, fDistance))
		{
			return false;
		}
	}

	if (m_pOwner)
	{

	CCollider_Cube* pOwnerCol = dynamic_cast<CCollider_Cube*>(m_pOwner->Get_Component(TEXT("Com_Collider_Cube")));
	if (CCthulhu::RayCubeIntersection(rayOrigin, rayDir, pOwnerCol, fDistance))
	{
		return false;
	}
	}
	return true;
}

void CCthulhu_Tentacle::Attack()
{
	auto vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.y += 0.8f;

	CTransform* pTransform = dynamic_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
	_float3 vDir = pTransform->Get_State(CTransform::STATE_POSITION) - vPos;
	vDir.Normalize();

	CCthulhuMissile::PROJ_DESC prjDesc{ vPos, vDir, 20.f };
	if(FAILED(m_pGameInstance->Add_GameObject(LEVEL_BOSS, TEXT("Prototype_GameObject_CthulhuMissile"), LEVEL_BOSS, TEXT("Layer_CthulhuMissile"), &prjDesc)))
		return;

	CCthulhuMissile* pMissile = static_cast<CCthulhuMissile*>(m_pGameInstance->Find_Last_Object(LEVEL_BOSS, TEXT("Layer_CthulhuMissile")));
	if (pMissile)
	{
	auto pTransform = static_cast<CTransform*>(pMissile->Get_Component(TEXT("Com_Transform")));
	if (pTransform)
	{
		pTransform->Set_Scale(0.7f, 0.7f, 1.f);
	}
	}

	m_eState = Tentacle_STATE::IDLE;
	m_fFrame = 0.f;
	m_iCurrentFrame = m_mapStateTextures[m_eState][(_uint)m_fFrame];
}

CCthulhu_Tentacle* CCthulhu_Tentacle::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCthulhu_Tentacle* pInstance = new CCthulhu_Tentacle(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to create CCthulhu_Tentacle instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCthulhu_Tentacle::Clone(void* pArg)
{
	CCthulhu_Tentacle* pClone = new CCthulhu_Tentacle(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("Failed to clone CCthulhu_Tentacle instance");
		Safe_Release(pClone);
	}
	return pClone;
}

void CCthulhu_Tentacle::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
}

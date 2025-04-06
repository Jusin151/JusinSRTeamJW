#include "Cthulhu.h"
#include <Player.h>
#include <StructureManager.h>
#include <CthulhuMissile.h>
#include <Camera_FirstPerson.h>

CCthulhu::CCthulhu(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster_Base(pGraphic_Device), m_pBehaviorTree(nullptr)
{
}

CCthulhu::CCthulhu(const CCthulhu& Prototype)
	: CMonster_Base(Prototype), m_pBehaviorTree(nullptr)
{
}

HRESULT CCthulhu::Initialize_Prototype()
{
	m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_GameObject_CthulhuMissile"), CCthulhuMissile::Create(m_pGraphic_Device));
	//	m_pGameInstance->Reserve_Pool(LEVEL_BOSS, TEXT("Prototype_GameObject_CthulhuMissile"),TEXT("Layer_CthulhuMissile"), 10);
	return S_OK;
}

HRESULT CCthulhu::Initialize(void* pArg)
{
	INIT_PARENT(pArg)

	if (FAILED(Ready_Components()))
		return E_FAIL;

	// 비헤이비어 트리 초기화
	m_pBehaviorTree = new CBehaviorTree();
	if (!m_pBehaviorTree)
		return E_FAIL;

	m_iHp = 500;
	m_fPhaseThreshold = { m_iHp * 0.5f };

	m_pBehaviorTree->Initialize();
	m_pTarget = m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_Player");
	Init_Textures();

	Create_BehaviorTree();

	_float3 pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,_float3(pos.x, -0.5f, pos.z));
	return S_OK;
}

HRESULT CCthulhu::Ready_Components()
{
	// 컴포넌트 초기화 코드
	if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Texture_Cthulhu"),
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

	ColliderDesc.fScale = { 11.f, 8.f, 10.f };
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f }; 
	m_eType = CG_MONSTER;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Cube"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

void CCthulhu::Init_Textures()
{
	for (_uint i = 0; i < m_pTextureCom->Get_NumTextures(); i++)
	{
		if (i < 7)
			m_mapStateTextures[STATE::IDLE].push_back(i);
		else if (i > 6 && i < 11)
		{
			m_mapStateTextures[STATE::ATTACK].push_back(i);
			m_mapStateTextures[STATE::MULTI_ATTACK].push_back(i);
		}
		else if (i > 10)
			m_mapStateTextures[STATE::DEAD].push_back(i);
	}
}

void CCthulhu::Update_Animation(_float fTimeDelta)
{
	m_fFrame += m_fAnimationSpeed * fTimeDelta;
	if (m_fFrame >= m_mapStateTextures[m_eState].size())
	{
		m_fFrame = 0.f;
	}
	m_iCurrentFrame = m_mapStateTextures[m_eState][(_uint)m_fFrame];
}

NodeStatus CCthulhu::Attack()
{
	if (m_bIsAttacking)
	{
		return NodeStatus::SUCCESS;
	}

	if (!IsPlayerVisible() || m_bIsMultiAttack)
	{
		return NodeStatus::FAIL;
	}

	if (m_fAttackCoolTime < m_fAttackCoolDown)
	{
		return NodeStatus::FAIL;
	}

	// 쿨타임이 완료되었으므로 타이머를 리셋
	m_fAttackCoolTime = 0.f;

	m_eState = STATE::ATTACK;
	m_fFrame = 0.f;
	m_iCurrentFrame = m_mapStateTextures[m_eState][(_uint)m_fFrame];

	m_iMissilesToFire = 3;  
	m_fMissileTimer = 0.f;  
	m_bIsAttacking = true;  

	return NodeStatus::SUCCESS;
}

NodeStatus CCthulhu::UpdateAttack()
{
	if (m_bIsAttacking)
	{
		m_eState = STATE::ATTACK;


		m_fMissileTimer += m_fDelta;
		if (m_fMissileTimer >= 0.2f && m_iMissilesToFire > 0)
		{

			auto vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vPos.y -= 1.2f;


			CTransform* pTransform = dynamic_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
			_float3 vDir = pTransform->Get_State(CTransform::STATE_POSITION) - vPos;
			vDir.Normalize();


			CCthulhuMissile::PROJ_DESC prjDesc{ vPos, vDir, 20.f };
			m_pGameInstance->Add_GameObject(LEVEL_BOSS, TEXT("Prototype_GameObject_CthulhuMissile"), LEVEL_BOSS, TEXT("Layer_CthulhuMissile"), &prjDesc);
			m_iMissilesToFire--;
			m_fMissileTimer = 0.f;
		}


		if (m_iMissilesToFire == 0)
		{
			m_eState = STATE::IDLE;
			m_fFrame = 0.f;
			m_iCurrentFrame = m_mapStateTextures[m_eState][(_uint)m_fFrame];
			m_bIsAttacking = false;
		}

		return NodeStatus::SUCCESS;
	}
	return NodeStatus::FAIL;
}

NodeStatus CCthulhu::MultiMissileAttack()
{
	if (!m_bIsMultiAttack && m_fMultiAttackCoolTime < m_fMultiAttackCoolDown)
	{
		return NodeStatus::FAIL;
	}

	if (m_iHp < m_fPhaseThreshold && IsPlayerVisible())
	{
		m_fMultiAttackCoolTime = 0.f;

		if (!m_bIsMultiAttack)
		{
			m_bIsMultiAttack = true;
			m_eState = STATE::MULTI_ATTACK;
			m_iMultiMissilesToFire = 7;
			m_fMultiMissileTimer = 0.f;
			m_fFrame = 0.f;
			m_iCurrentFrame = m_mapStateTextures[m_eState][0];
		}

		if (m_bIsMultiAttack)
		{
			m_fMultiMissileTimer += m_fDelta;
		}

		if (m_fMultiMissileTimer >= 0.1f && m_iMultiMissilesToFire > 0)
		{
			auto vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			vPos.y -= 1.2f;
			CTransform* pTransform = dynamic_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
			_float3 vDir = pTransform->Get_State(CTransform::STATE_POSITION) - vPos;
			vDir.Normalize();

			CCthulhuMissile::PROJ_DESC prjDesc{ vPos, vDir, 60.f };
			m_pGameInstance->Add_GameObject(LEVEL_BOSS, TEXT("Prototype_GameObject_CthulhuMissile"),
				LEVEL_BOSS, TEXT("Layer_CthulhuMissile"), &prjDesc);

			m_iMultiMissilesToFire--;
			m_fMultiMissileTimer = 0.f;
		}

		if (m_iMultiMissilesToFire == 0)
		{
			m_bIsMultiAttack = false;
			m_eState = STATE::IDLE;
			m_fFrame = 0.f;
			m_iCurrentFrame = m_mapStateTextures[m_eState][0];
		}
		return NodeStatus::SUCCESS;
	}
	return NodeStatus::FAIL;
}

NodeStatus CCthulhu::Update_Appear()
{
	if (m_bIsAppeared)
		return NodeStatus::FAIL;



	const float appearSpeed = 2.2f;   
	const float targetY = 4.5f;      

	if (!m_bCameraShaken)  // 이미 쉐이크 효과가 발생하지 않았다면
	{
		CCamera_FirstPerson* pCamera = dynamic_cast<CCamera_FirstPerson*>(m_pGameInstance->Find_Object(LEVEL_STATIC,TEXT("Layer_Camera")));
		if (pCamera)
		{
			pCamera->TriggerShake(0.2f, 4.f);
			m_bCameraShaken = true;
		}
	}
	_float3 pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	pos.y += appearSpeed * m_fDelta;

	if (pos.y >= targetY)
	{
		pos.y = targetY;
		m_bIsAppeared = true; 
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pos);

		return NodeStatus::SUCCESS;
	}
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pos);
		return NodeStatus::RUNNING;
}

void CCthulhu::Create_BehaviorTree()
{
	CCompositeNode* pStateSelector = new CSelectorNode();
	CCompositeNode* pAttackSelector = new CSelectorNode();
	CCompositeNode* pAttackSeq = new CSequenceNode();

	TaskNode* pMultiAttack = new TaskNode(L"MultiMissileAttack", [this]() -> NodeStatus { return this->MultiMissileAttack(); });

	TaskNode* pAttack = new TaskNode(L"Attack", [this]() -> NodeStatus { return this->Attack(); });
	TaskNode* pUpdateAttack = new TaskNode(L"UpdateAttack", [this]() -> NodeStatus {return this->UpdateAttack(); });
	pAttackSeq->AddChild(pAttack);
	pAttackSeq->AddChild(pUpdateAttack);

	pAttackSelector->AddChild(pMultiAttack);
	pAttackSelector->AddChild(pAttackSeq);

	pStateSelector->AddChild(new TaskNode(L"UpdateAppear", [this]() -> NodeStatus { return this->Update_Appear(); }));
	pStateSelector->AddChild(new TaskNode(L"Dead", [this]() -> NodeStatus {

		if (m_iHp > 0)
			return NodeStatus::FAIL;

		if (m_eState != STATE::DEAD)
		{
			m_eState = STATE::DEAD;
			m_fFrame = 0.f;
			m_iCurrentFrame = m_mapStateTextures[m_eState][0];
		}

		if (m_fFrame < m_mapStateTextures[STATE::DEAD].size() - 1)
			return NodeStatus::RUNNING;
		else
		{
			if (m_bUpdateAnimation)
			{
				m_bUpdateAnimation = false;
			}
			return NodeStatus::SUCCESS;
		}
		}));
	pStateSelector->AddChild(pAttackSelector);

	m_pBehaviorTree->Set_Root(pStateSelector);
}

_bool CCthulhu::RayCubeIntersection(const _float3& rayOrigin, _float3& rayDir, CCollider_Cube* pCollider, _float fMaxDistance)
{
	if (pCollider == nullptr)
		return false;

	_float fScaleX = pCollider->Get_Desc().fAxisX.Length();
	_float fScaleY = pCollider->Get_Desc().fAxisY.Length();
	_float fScaleZ = pCollider->Get_Desc().fAxisZ.Length();

	_float3 vDirX = pCollider->Get_Desc().fAxisX.GetNormalized();
	_float3 vDirY = pCollider->Get_Desc().fAxisY.GetNormalized();
	_float3 vDirZ = pCollider->Get_Desc().fAxisZ.GetNormalized();

	_float fHalfSize[3] = { fScaleX * 0.5f, fScaleY * 0.5f, fScaleZ * 0.5f };
	_float3 vDir[3] = { vDirX, vDirY, vDirZ };

	_float3 vLocalOrigin = rayOrigin - pCollider->Get_Desc().fPos;

	_float3 vLocalDir, vLocalPos;
	for (int i = 0; i < 3; i++)
	{
		vLocalDir[i] = rayDir.Dot(vDir[i]);
		vLocalPos[i] = vLocalOrigin.Dot(vDir[i]);
	}

	_float fMin = -FLT_MAX;
	_float fMax = FLT_MAX;

	for (int i = 0; i < 3; i++)
	{
		if (fabs(vLocalDir[i]) < FLT_EPSILON)
    {
			if (vLocalPos[i] < -fHalfSize[i] || vLocalPos[i] > fHalfSize[i])
				return false;
		}
		else
		{
			float t1 = (-fHalfSize[i] - vLocalPos[i]) / vLocalDir[i];
			float t2 = (fHalfSize[i] - vLocalPos[i]) / vLocalDir[i];

			if (t1 > t2)
			{
				float temp = t1;
				t1 = t2;
				t2 = temp;
			}

			fMin = max(fMin, t1);
			fMax = min(fMax, t2);

			if (fMin > fMax)
				return false;
		}
	}
	return (fMin <= fMax && fMax >= 0.0f && fMin < fMaxDistance);
}

_bool CCthulhu::IsPlayerVisible()
{
	_float3 monsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

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

		if (RayCubeIntersection(rayOrigin, rayDir, pWallCollider, fDistance))
		{
			return false;
		}
	}
	return true;
}

HRESULT CCthulhu::On_Collision(CCollisionObject* other)
{
	return S_OK;
}


void CCthulhu::Select_Pattern(_float fTimeDelta)
{
  // 비헤이비어 트리 실행
	if (m_pBehaviorTree)
 		m_pBehaviorTree->Run();
}

json CCthulhu::Serialize()
{
	json j = __super::Serialize();

	auto pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	auto scale = m_pTransformCom->Compute_Scaled();
	auto angle = m_pTransformCom->Get_EulerAngles();
	j["position"] = {
	RoundToDecimalPlaces(pos.x, 2),
	RoundToDecimalPlaces(pos.y, 2),
	RoundToDecimalPlaces(pos.z, 2)
	};
	j["rotation"] = {
		RoundToDecimalPlaces(angle.x, 2),
		RoundToDecimalPlaces(angle.y, 2),
		RoundToDecimalPlaces(angle.z, 2)
	};
	j["scale"] = {
		RoundToDecimalPlaces(scale.x, 2),
		RoundToDecimalPlaces(scale.y, 2),
		RoundToDecimalPlaces(scale.z, 2)
	};

	return j;
}

void CCthulhu::Deserialize(const json& j)
{
	SET_TRANSFORM(j, m_pTransformCom);
}
void CCthulhu::Priority_Update(_float fTimeDelta)
{
	m_fDelta = fTimeDelta;

	m_fAttackCoolTime += fTimeDelta;
	m_fMultiAttackCoolTime += fTimeDelta;

	Select_Pattern(fTimeDelta);
}

void CCthulhu::Update(_float fTimeDelta)
{
	if (m_bIsAppeared&&m_pColliderCom)
	{
		m_pColliderCom->Set_WorldMat(m_pTransformCom->Get_WorldMat()); 

		m_pColliderCom->Update_Collider_Boss(TEXT("Com_Collider_Cube"));


		m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
	}


	Billboarding(fTimeDelta);
	if (m_bUpdateAnimation)
	{
		Update_Animation(fTimeDelta);
	}
}

void CCthulhu::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CCthulhu::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 100);
	return S_OK;
}

HRESULT CCthulhu::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CCthulhu::Render()
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

	if (g_bDebugCollider)
	{
		m_pColliderCom->Render();
	}

	Release_RenderState();

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("현재 HP :") + to_wstring(m_iHp),
		_float2(-300.f, 0.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	return S_OK;
}


void CCthulhu::Billboarding(_float fTimeDelta)
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

CCthulhu* CCthulhu::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCthulhu* pInstance = new CCthulhu(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to create CCthulhu instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCthulhu::Clone(void* pArg)
{
	CCthulhu* pClone = new CCthulhu(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("Failed to clone CCthulhu instance");
		Safe_Release(pClone);
	}
	return pClone;
}

void CCthulhu::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Delete(m_pBehaviorTree);
}



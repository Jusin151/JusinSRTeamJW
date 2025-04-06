#include "Cthulhu.h"
#include <Player.h>
#include <CthulhuMissile.h>

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
	//m_pGameInstance->Reserve_Pool(LEVEL_BOSS, TEXT("Prototype_GameObject_CthulhuMissile"),TEXT("Layer_CthulhuMissile"), 10);
	return S_OK;
}

HRESULT CCthulhu::Initialize(void* pArg)
{
	INIT_PARENT(pArg)
		
	m_eType = CG_MONSTER;
	if (FAILED(Ready_Components()))
		return E_FAIL;

	// 비헤이비어 트리 초기화
	m_pBehaviorTree = new CBehaviorTree();
	if (!m_pBehaviorTree)
		return E_FAIL;

	m_pBehaviorTree->Initialize();
	m_pTarget = m_pGameInstance->Find_Object(LEVEL_STATIC,L"Layer_Player");
	Init_Textures();

	Create_BehaviorTree();
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

	ColliderDesc.fScale = { 1.f, 1.f, 1.f };
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };
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
			m_mapStateTextures[STATE::ATTACK].push_back(i);
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
	if (m_fAttackCoolTime < m_fAttackCoolDown)
		return NodeStatus::FAIL;

	// 쿨타임이 완료되었으므로 타이머를 리셋
	m_fAttackCoolTime = 0.f;

	m_eState = STATE::ATTACK;
	m_fFrame = 0.f;
	m_iCurrentFrame = m_mapStateTextures[m_eState][(_uint)m_fFrame];

	// 미사일 발사를 위한 위치 보정
	auto vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.y -= 1.f;
	CTransform* pTransform = dynamic_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
	_float3 vDir = pTransform->Get_State(CTransform::STATE_POSITION) - vPos;
	vDir.Normalize();
	CCthulhuMissile::PROJ_DESC prjDesc{ vPos, vDir, 4.f };
	//m_pGameInstance->Add_GameObject_FromPool(LEVEL_BOSS, LEVEL_BOSS, TEXT("Layer_CthulhuMissile"), &prjDesc);
	m_pGameInstance->Add_GameObject(LEVEL_BOSS, TEXT("Prototype_GameObject_CthulhuMissile"), LEVEL_BOSS, TEXT("Layer_CthulhuMissile"), &prjDesc);

	return NodeStatus::SUCCESS;
}
void CCthulhu::Create_BehaviorTree()
{
	CCompositeNode* pRoot = new CSequenceNode();
	TaskNode* pAttack = new TaskNode(L"Attack", [this]() -> NodeStatus { return this->Attack(); });
	pRoot->AddChild(pAttack);

	m_pBehaviorTree->Set_Root(pRoot);	

	//// 비헤이비어 트리 생성 및 초기화
	//m_pBehaviorTree->Create_Sequence("CthulhuAttack");
	//m_pBehaviorTree->Create_Selector("CthulhuIdle");
	//m_pBehaviorTree->Create_Condition("CthulhuIdle", "CthulhuAttack", "IsPlayerInRange");
	//m_pBehaviorTree->Create_Action("CthulhuAttack", "Attack");
	//m_pBehaviorTree->Create_Action("CthulhuIdle", "Idle");
	//m_pBehaviorTree->Create_Condition("CthulhuIdle", "IsPlayerInRange");
}
HRESULT CCthulhu::On_Collision(CCollisionObject* other)
{
	return S_OK;
}


void CCthulhu::Select_Pattern(_float fTimeDelta)
{// 비헤이비어 트리 실행
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
	Billboarding(fTimeDelta);
	// 비헤이비어 트리 실행
	if (m_pBehaviorTree)
		m_pBehaviorTree->Run();
}

void CCthulhu::Update(_float fTimeDelta)
{
	if (m_pColliderCom)
	{
	m_pColliderCom->Set_WorldMat(m_pTransformCom->Get_WorldMat());

	m_pColliderCom->Update_Collider(TEXT("Com_Collider_Cube"), m_pTransformCom->Compute_Scaled());
	m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
	}
	m_fAttackCoolTime += fTimeDelta;
	Select_Pattern(fTimeDelta);
	Billboarding(fTimeDelta);
	Update_Animation(fTimeDelta);
}

void CCthulhu::Late_Update(_float fTimeDelta)
{
	static _bool test = false;
	if (!test)
	{
		for(int i = 0;i<5;i++)
		Attack();

		test = true;
	}
	m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CCthulhu::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF,100);
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

	return S_OK;
}


void CCthulhu::Billboarding(_float fTimeDelta)
{
	if (!m_pTarget) return;


	CTransform* pPlayerTransform = static_cast<CPlayer*>(m_pTarget)->Get_TransForm();

	_float3 vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);

	vPlayerLook.y = 0.f;
	vPlayerLook.Normalize();

	_float3 vShopLook = -vPlayerLook;


	_float3 vUp = _float3(0.0f, 1.0f, 0.0f);
	_float3 vRight = vUp.Cross(vShopLook);
	vRight.Normalize();

	_float3 vNewUp = vShopLook.Cross(vRight);
	vNewUp.Normalize();

	_float3 vScale = m_pTransformCom->Compute_Scaled();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vNewUp * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vShopLook * vScale.z);
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
	// 비헤이비어 트리 해제
	Safe_Release(m_pTextureCom);
	Safe_Delete(m_pBehaviorTree);
}



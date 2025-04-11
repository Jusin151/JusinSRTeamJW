#include "Cthulhu.h"
#include <Player.h>
#include <StructureManager.h>
#include <CthulhuMissile.h>
#include <Camera_FirstPerson.h>
#include "Cthulhu_Tentacle.h"
#include "Cthulhu_Big_Tentacle.h"

CCthulhu::CCthulhu(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster_Base(pGraphic_Device), m_pBehaviorTree(nullptr),
	m_bIsColned(false)
{
	m_vecBigTentacles.reserve(4);
}

CCthulhu::CCthulhu(const CCthulhu& Prototype)
	: CMonster_Base(Prototype), m_pBehaviorTree(nullptr),
	m_bIsColned(true),
	m_listTentacles(Prototype.m_listTentacles),
	m_iCountTentacle(Prototype.m_iCountTentacle),
	m_vecBigTentacles(Prototype.m_vecBigTentacles),
	m_vecSpikes(Prototype.m_vecSpikes)
{
}

HRESULT CCthulhu::Initialize_Prototype()
{
	auto pTentacle = CCthulhu_Tentacle::Create(m_pGraphic_Device);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_GameObject_Cthulhu_Tentacle"), pTentacle)))
	{
		Safe_Release(pTentacle);
		return E_FAIL;
	}

	for (int i = 0; i < 8; i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_BOSS, TEXT("Prototype_GameObject_Cthulhu_Tentacle"), LEVEL_BOSS, TEXT("Layer_Cthulhu_Tentacle"))))
			continue;
		m_listTentacles.push_back(dynamic_cast<CCthulhu_Tentacle*>(m_pGameInstance->Find_Last_Object(LEVEL_BOSS, TEXT("Layer_Cthulhu_Tentacle"))));
		if (m_listTentacles.back())
		{
			Safe_AddRef(m_listTentacles.back());
			m_listTentacles.back()->SetActive(false);
		}
		else
		{
			m_listTentacles.pop_back();
		}
	}

	auto pBigTentacle = CCthulhu_Big_Tentacle::Create(m_pGraphic_Device);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_GameObject_Cthulhu_BigTentacle"), pBigTentacle)))
	{
		Safe_Release(pTentacle);
		return E_FAIL;
	}

	for (size_t i = 0; i < m_vecBigTentacles.capacity(); i++)
	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_BOSS, TEXT("Prototype_GameObject_Cthulhu_BigTentacle"), LEVEL_BOSS, TEXT("Layer_Cthulhu_BigTentacle"))))
			continue;
		m_vecBigTentacles.push_back((dynamic_cast<CCthulhu_Big_Tentacle*>(m_pGameInstance->Find_Last_Object(LEVEL_BOSS, TEXT("Layer_Cthulhu_BigTentacle")))));
		if (m_vecBigTentacles.back())
		{
			Safe_AddRef(m_vecBigTentacles.back());
			m_vecBigTentacles.back()->SetActive(false);
			m_vecBigTentacles.back()->Set_Texture((i + 1) % 2 == 0);
		}
		else
		{
			m_vecBigTentacles.pop_back();
		}
	}

	auto pMissile = CCthulhuMissile::Create(m_pGraphic_Device);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_GameObject_CthulhuMissile"), pMissile)))
	{
		Safe_Release(pMissile);
		return E_FAIL;
	}

	auto pSpike = CSpike::Create(m_pGraphic_Device);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS, TEXT("Prototype_GameObject_Cthulhu_Spike"), pSpike)))
	{
		Safe_Release(pSpike);
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Reserve_Pool(LEVEL_BOSS, TEXT("Prototype_GameObject_Cthulhu_Spike"), TEXT("Layer_Cthulhu_Spike"), 54)))
	{
		return E_FAIL;
	}

	m_vecSpikes.reserve(50);

	m_iCountTentacle = (_int)m_listTentacles.size();
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

	m_iHp = 2000;
	m_fPhaseThreshold = { m_iHp * 0.6f };
	m_fPhaseThreshold2 = { m_iHp * 0.3f };

	m_pBehaviorTree->Initialize();
	m_pTarget = m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_Player");
	Init_Textures();

	Create_BehaviorTree();

	m_pPlayerTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));

	_float3 pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(pos.x, -0.5f, pos.z));
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

	ColliderDesc.fScale = { 10.f, 8.5f, 3.f };
	ColliderDesc.fLocalPos = { 0.f, -0.3f, 0.f };
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

void CCthulhu::Init_Textures()
{
	for (_uint i = 0; i < m_pTextureCom->Get_NumTextures(); i++)
	{
		if (i < 7)
		{
			m_mapStateTextures[STATE::IDLE].push_back(i);
			m_mapStateTextures[STATE::DPELOY_TENTACLES].push_back(i);
		}
		else if (i > 6 && i < 11)
		{
			m_mapStateTextures[STATE::ATTACK].push_back(i);
			m_mapStateTextures[STATE::MULTI_ATTACK].push_back(i);
		}
		else if (i > 10)
		{
			m_mapStateTextures[STATE::DEAD].push_back(i);
		}
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

	if (!m_bCameraShaken)
	{
		CCamera_FirstPerson* pCamera = dynamic_cast<CCamera_FirstPerson*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Camera")));
		if (pCamera)
		{
			pCamera->TriggerShake(0.2f, 4.5f);
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

NodeStatus CCthulhu::Deploy_Tentacles()
{
	if (m_listTentacles.empty())
	{
		if (!m_bCanHit) m_bCanHit = true;
		return NodeStatus::FAIL;
	}
	if (m_bIsTentacleInstalled)
	{
		m_fTentacleTimer += m_fDelta;

		if (m_fTentacleTimer >= 5.0f)
		{
			for (auto& pTentacle : m_listTentacles)
			{
				if (pTentacle && pTentacle->Get_State() != CCthulhu_Tentacle::Tentacle_STATE::DEAD)
				{
					if (pTentacle->Get_State() != CCthulhu_Tentacle::Tentacle_STATE::DISAPPEAR)
					{
						pTentacle->Set_State(CCthulhu_Tentacle::Tentacle_STATE::DISAPPEAR);
					}
				}
			}

			_bool bAllFinished = true;
			for (auto& pTentacle : m_listTentacles)
			{
				if (pTentacle && pTentacle->Get_State() == CCthulhu_Tentacle::Tentacle_STATE::DISAPPEAR)
				{
					if (!pTentacle->IsAnimationFinished())
					{
						bAllFinished = false;
						break;
					}
				}
			}
			// 모든 촉수가 사라짐 애니메이션을 완료했다면
			if (bAllFinished)
			{
				m_bIsTentacleInstalled = false;   // 촉수 활성화 플래그 해제
				m_fTentacleTimer = 0.f;             // 활성화 타이머 초기화
				return NodeStatus::SUCCESS;         // 사라짐 처리 완료
			}
			else
			{
				return NodeStatus::RUNNING;
			}
		}
		return NodeStatus::RUNNING;
	}

	if (m_fTentacleCoolTime < 7.f)
	{
		return NodeStatus::FAIL;
	}

	m_fTentacleCoolTime = 0.f;       // 쿨다운 초기화
	m_bIsTentacleInstalled = true;   // 촉수 활성화
	m_fTentacleTimer = 0.f;
	m_eState = STATE::DPELOY_TENTACLES;

	for (auto it = m_listTentacles.begin(); it != m_listTentacles.end(); )
	{
		CCthulhu_Tentacle* pTentacle = *it;


		if (pTentacle == nullptr
			|| pTentacle->Get_State() == CCthulhu_Tentacle::Tentacle_STATE::DEAD)
		{
			Safe_Release(pTentacle);
			it = m_listTentacles.erase(it);
			m_iCountTentacle--;
		}
		else
		{
			pTentacle->SetActive(true);

			// -25 ~ +25 범위 랜덤 X, Z
			_float randomX = (static_cast<_float>(rand()) / RAND_MAX) * 50.f - 25.f;
			_float randomZ = (static_cast<_float>(rand()) / RAND_MAX) * 50.f - 25.f;

			_float3 currentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float3 newPos = { currentPos.x + randomX,
								currentPos.y,
								currentPos.z + randomZ };
			pTentacle->Set_Position(newPos);
			pTentacle->Set_State(CCthulhu_Tentacle::Tentacle_STATE::APPEAR);

			++it;
		}
	}
	return NodeStatus::RUNNING;
}

NodeStatus CCthulhu::Deploy_BigTentacles()
{

	if (m_eState == STATE::DEAD)
		return NodeStatus::FAIL;

	m_fBigTentacleTimer += m_fDelta;
	const float interval = 7.0f;
	if (m_fBigTentacleTimer >= interval)
	{
		_float3 basePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		const _float offsetX = 6.5f;
		const _float offsetZ = 15.0f;

		const _float2 offsets[4] = {
			{ -offsetX, -offsetZ },  // 왼쪽 아래
			{ -offsetX, +offsetZ },  // 왼쪽 위
			{ +offsetX, -offsetZ },  // 오른쪽 아래
			{ +offsetX, +offsetZ }   // 오른쪽 위
		};
		_int order[4] = { 0, 3, 1, 2 };
		random_shuffle(begin(order), end(order));
		for (int i = 0; i < 4; ++i)
		{
			int idx = order[i];

			auto pTentacle = m_vecBigTentacles[idx];
			if (!pTentacle)
				continue;

			_float3 newPos = {
				basePos.x + offsets[i].x,
				basePos.y,
				basePos.z + offsets[i].y
			};

			if (!pTentacle->IsActive())
				pTentacle->SetActive(true);
			pTentacle->Set_Position(newPos);
		}

		m_fBigTentacleTimer = 0.0f;
	}

	return NodeStatus::RUNNING;
}

NodeStatus CCthulhu::Attack_Spike()
{
	if (m_eState == STATE::DEAD)// || static_cast<_float>(m_iHp)> m_fPhaseThreshold2)
		return NodeStatus::FAIL;
	static _uint iIndex = 0;
	static _bool bIsCircle = false;
	static _float fWaringTime = 0.f;
	if (m_bSpikeAppeared)
	{
		fWaringTime += m_fDelta;
		m_fSpikeTimer += m_fDelta;

		if (fWaringTime >= 1.2f)
		{


		_float fSpikeTimer = (bIsCircle ? 0.2f : 0.09f);
		if (m_fSpikeTimer >= fSpikeTimer)
		{
			if (!bIsCircle)
			{

				// 마지막 스파이크가 아니라면 다음 스파이크로 진행
				if (iIndex < m_vecSpikes.size())
				{
					if (iIndex == m_vecSpikes.size() - 1)
					{
						m_vecSpikes[iIndex]->ActiveShakeTrigger();
					}
					m_vecSpikes[iIndex]->Active_Animaiton(true);
					iIndex++;

				}
			}
			else
			{

				for (_uint j = iIndex; j < iIndex + 18 && j < m_vecSpikes.size(); j++)
				{
					m_vecSpikes[j]->Active_Animaiton(true);
				}
				iIndex += 18; 
			}

			if (iIndex == m_vecSpikes.size())
			{
				m_bSpikeAppeared = false;
				iIndex = 0;
				fWaringTime = 0.f;
				m_vecSpikes.clear();
				return NodeStatus::SUCCESS;
			}
		
			m_fSpikeTimer = 0.f;
		}

		}

		return NodeStatus::RUNNING;
	}

	if (m_fSpikeCoolTime < 5.f)
	{
		return NodeStatus::FAIL;
	}

	m_fSpikeCoolTime = 0.f;
	m_bSpikeAppeared = true;
	m_fSpikeTimer = 0.f;
	m_eState = STATE::IDLE;

	static _bool bFirstTime = true;
	static _float3 vScale;
	static const _float s_fSpacingOffset = 3.5f;

	_float3 basePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (m_pPlayerTransform)
	{
		_float3 vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_float3 vPlayerRight = m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT).GetNormalized();

		_float fDistance = _float3::Distance(vPlayerPos, basePos);
		_float3 vDir = (vPlayerPos - basePos).GetNormalized();

		_float3 vSpikeDirFromPlayer = (basePos - vPlayerPos).GetNormalized();
		// 플레이어의 right 벡터와의 내적으로 좌우 판단
		// dot 값이 양이면 스파이크 생성 기준이 플레이어 기준 오른쪽, 음수면 왼쪽
		_float fDotVal = vSpikeDirFromPlayer.Dot(vPlayerRight);

		const _float centerThreshold = 0.2f;

		if (bFirstTime)
		{
			CSpike* pSpike = static_cast<CSpike*>(m_pGameInstance->Add_GameObject_FromPool(LEVEL_BOSS, LEVEL_BOSS, TEXT("Layer_Cthulhu_Spike")));
			if (CTransform* pSpikeTransform = static_cast<CTransform*>(pSpike->Get_Component(TEXT("Com_Transform"))))
			{
				vScale = pSpikeTransform->Compute_Scaled();
			}
			bFirstTime = false;
		}

		_uint iSpikeCount = max(1u, static_cast<_uint>(ceil(fDistance / (2.f + s_fSpacingOffset)))) + 1;

		if (fDistance >=14.f)
		{
			bIsCircle = false;
		for (_uint i = 0; i < iSpikeCount; i++)
		{
			if (CSpike* pNewSpike = static_cast<CSpike*>(m_pGameInstance->Add_GameObject_FromPool(LEVEL_BOSS, LEVEL_BOSS, TEXT("Layer_Cthulhu_Spike"))))
			{

				pNewSpike->Active_Animaiton(false);
				_float3 newPos = basePos + vDir * ((2.f + s_fSpacingOffset) * i);
				if (i == 0)
					newPos.z += 0.01f;

				if (abs(fDotVal) <= centerThreshold)
				{
					pNewSpike->Set_SpikeType(CSpike::SPIKE_TYPE::CENTER);
				}
				else
				{
					pNewSpike->Set_SpikeType(CSpike::SPIKE_TYPE::SIDE);
					// dot 값의 부호에 따라 플레이어 기준 오른쪽이면 Flip(true), 왼쪽이면 Flip(false)
					pNewSpike->Flip(fDotVal > 0.f);
				}
				pNewSpike->Set_Position(newPos);
				pNewSpike->Activate_WarningZone();
				m_vecSpikes.push_back(pNewSpike);
			}
			else
			{
				return NodeStatus::FAIL;
			}
		}

		}
		else
		{
			bIsCircle = true;
		_uint uiTotalSpikes = 18; // 배치할 스파이크 개수
		_float fRadius = 7.5f;    // 원의 반지름
		
		for (_uint i = 0; i < 3; i++)
		{

			for (_uint i = 0; i < uiTotalSpikes; i++)
			{
				if (CSpike* pNewSpike = static_cast<CSpike*>(m_pGameInstance->Add_GameObject_FromPool(LEVEL_BOSS, LEVEL_BOSS, TEXT("Layer_Cthulhu_Spike"))))
				{
					pNewSpike->Active_Animaiton(false);

					_float fAngle = (2.f * D3DX_PI * i) / uiTotalSpikes;

					// 원형 배치를 위한 위치 계산
					_float3 vNewPos;
					vNewPos.x = basePos.x + fRadius * cosf(fAngle);
					vNewPos.y = basePos.y;
					vNewPos.z = basePos.z + fRadius * sinf(fAngle);

					pNewSpike->Set_Position(vNewPos);
					pNewSpike->Activate_WarningZone();

					_float fAngleDeg = D3DXToDegree(fAngle);
					if (fAngleDeg < 0.f)
						fAngleDeg += 360.f;

					// 상/하/좌/우 영역 구분
					// [315, 360) && [0, 45) => 상 (CENTER)
					// [45, 135) => 오른쪽 (RIGHT)
					// [135, 225) => 하 (CENTER)
					// [225, 315) => 왼쪽 (LEFT)

					if ((fAngleDeg >= 315.f && fAngleDeg < 360.f) ||
						(fAngleDeg >= 0.f && fAngleDeg < 45.f))
					{
						// 위쪽
						pNewSpike->Set_SpikeType(CSpike::SPIKE_TYPE::CENTER);
					}
					else if (fAngleDeg >= 45.f && fAngleDeg < 135.f)
					{
						// 오른쪽
						pNewSpike->Set_SpikeType(CSpike::SPIKE_TYPE::SIDE);
						pNewSpike->Flip(true);
					}
					else if (fAngleDeg >= 135.f && fAngleDeg < 225.f)
					{
						// 아래쪽
						pNewSpike->Set_SpikeType(CSpike::SPIKE_TYPE::CENTER);
					}
					else
					{
						// 왼쪽
						pNewSpike->Set_SpikeType(CSpike::SPIKE_TYPE::SIDE);
						pNewSpike->Flip(false);
					}
					m_vecSpikes.push_back(pNewSpike);
				}
				else
				{
					return NodeStatus::FAIL;
				}
			}
			fRadius += 2.3f;
		}
		}
	}

	

	return NodeStatus::RUNNING;
}

void CCthulhu::Create_BehaviorTree()
{
	CCompositeNode* pStateSelector = new CSelectorNode();
	CCompositeNode* pAttackParallel = new CParallelNode();

	CCompositeNode* pAttackSeq = new CSequenceNode();
	TaskNode* pAttack = new TaskNode(L"Attack", [this]() -> NodeStatus { return this->Attack(); });
	TaskNode* pUpdateAttack = new TaskNode(L"UpdateAttack", [this]() -> NodeStatus { return this->UpdateAttack(); });
	pAttackSeq->AddChild(pAttack);
	pAttackSeq->AddChild(pUpdateAttack);

	TaskNode* pMultiAttack = new TaskNode(L"MultiMissileAttack", [this]() -> NodeStatus { return this->MultiMissileAttack(); });
	TaskNode* pDeploy = new TaskNode(L"Deploy", [this]() -> NodeStatus { return this->Deploy_Tentacles(); });
	TaskNode* pBigDeploy = new TaskNode(L"Big_Deploy", [this]() -> NodeStatus { return this->Deploy_BigTentacles(); });
	TaskNode* pSpikeAttack = new TaskNode(L"Attack_Spike", [this]() -> NodeStatus { return this->Attack_Spike(); });

	// 동시에 실행
	pAttackParallel->AddChild(pBigDeploy);
	pAttackParallel->AddChild(pDeploy);
	pAttackParallel->AddChild(pAttackSeq);
	pAttackParallel->AddChild(pMultiAttack);
	pAttackParallel->AddChild(pSpikeAttack);


	pStateSelector->AddChild(new TaskNode(L"UpdateAppear", [this]() -> NodeStatus {
		return this->Update_Appear();
		}));

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
				m_bUpdateAnimation = false;
			return NodeStatus::SUCCESS;
		}
		}));

	pStateSelector->AddChild(pAttackParallel);

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

	if (m_bIsAppeared)
	{
	m_fAttackCoolTime += fTimeDelta;
	m_fMultiAttackCoolTime += fTimeDelta;
	m_fTentacleCoolTime += fTimeDelta;
	m_fBigTentacleCoolTime += fTimeDelta;
	m_fSpikeCoolTime += fTimeDelta;
	}
	Select_Pattern(fTimeDelta);
}

void CCthulhu::Update(_float fTimeDelta)
{
	if (m_bIsAppeared && m_pColliderCom)
	{
		m_pColliderCom->Set_WorldMat(m_pTransformCom->Get_WorldMat());

		m_pColliderCom->Update_Collider_Boss(TEXT("Com_Collider_Cube"));


		if (m_bCanHit)
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
	D3DXVECTOR2 vScaleFactor(1.f, 1.f);
	D3DXVECTOR2 vOffsetFactor(0.0f, 0.0f); // Y축 반전을 위한 오프셋 조정
	m_pShaderCom->Set_UVScaleFactor(&vScaleFactor);
	m_pShaderCom->Set_UVOffsetFactor(&vOffsetFactor);

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
	m_pShaderCom->Set_Fog(_float3(0.247f, 0.55f, 0.407f), 1.f, 60.f);
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

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("현재 HP :") + to_wstring(m_iHp),
		_float2(-300.f, 0.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("현재 남은 촉수 :") + to_wstring(m_iCountTentacle),
		_float2(-200.f, 0.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	_float3 basePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_float fDistance = _float3::Distance(vPlayerPos, basePos);

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("현재 거리 :") + to_wstring(fDistance),
		_float2(0.f, 0.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));
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

	if (m_bIsColned)
	{
		for (auto& pTentacle : m_listTentacles)
		{
			Safe_Release(pTentacle);
		}
		m_listTentacles.clear();

		for (auto& pTentacle : m_vecBigTentacles)
		{
			Safe_Release(pTentacle);
		}
		m_vecBigTentacles.clear();
	}
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMaterialCom);
	Safe_Release(m_pShaderCom);
	Safe_Delete(m_pBehaviorTree);
}



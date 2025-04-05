#include "HellBoss.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Collider_Cube.h"
#include "Transform.h"
#include "Player.h"
#include "GameInstance.h"

CHellBoss::CHellBoss(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CMonster_Base(pGraphic_Device)
{
}

CHellBoss::CHellBoss(const CHellBoss& Prototype)
	:CMonster_Base(Prototype)
{
}

HRESULT CHellBoss::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CHellBoss::Initialize(void* pArg)
{


	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_eType = CG_MONSTER;

	m_iAp = 5;

	m_iHp = 30;

	m_fSpeed = 0.15f;

	m_pColliderCom->Set_Scale(_float3(2.f, 2.f, 2.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(7.f, 3.f, -5.f));

	m_pTransformCom->Set_Scale(12.f, 14.f, 10.f);


	m_TextureRanges["1_Idle"] = { 0, 0 };
	m_TextureRanges["2_Walk"] = { 1, 8 };
	m_TextureRanges["3_EyeBlast"] = { 9, 29 };
	m_TextureRanges["4_Shoot"] = { 30, 55 };
	m_TextureRanges["5_Morph"] = { 56, 86 };

	m_TextureRanges["6_Phase2_Idle"] = { 86, 86 };
	m_TextureRanges["7_Phase2_Walk"] = { 87, 94 };
	m_TextureRanges["8_Phase2_Charge"] = { 95, 99 };
	m_TextureRanges["9_Phase2_Spin"] = { 100, 103 };
	m_TextureRanges["0_Phase2_Shoot"] = { 104, 107 };
	m_TextureRanges["T_Phase2_End"] = { 108, 111 };

	m_TextureRanges["Y_ArmCut"] = { 112, 117 };
	m_TextureRanges["U_ArmCut_Idle"] = { 117, 117 };
	m_TextureRanges["I_ArmCut_Walk"] = { 118, 124 };
	m_TextureRanges["P_ArmCut_Attack"] = { 125, 138 };

	m_TextureRanges["G_Phase3_Idle"] = { 203, 203 };
	m_TextureRanges["H_Phase3_Search"] = { 204, 212 };
	m_TextureRanges["J_Phase3_TripleEye"] = { 213, 223 };
	m_TextureRanges["K_Phase3_Nova"] = { 224, 234 };
	m_TextureRanges["L_Phase3_Spawn"] = { 235, 246 };

	m_TextureRanges["O_Phase4_Idle"] = { 290, 311 };
	m_TextureRanges["M_Phase4_Death"] = { 312, 337 };


	return S_OK;
}

void CHellBoss::Priority_Update(_float fTimeDelta)
{
 	if (nullptr == m_pTarget)
	{
		CGameObject* pTarget = m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player"));
		if (nullptr == pTarget)
			return;

		SetTarget(pTarget);
		Safe_AddRef(pTarget);
		
	}

	//if (!m_bCheck)
	//{
	//	if (m_pTrigger == static_cast<CCollisionObject*>(m_pTarget)->Get_Trigger())
	//		m_bCheck = true;
	//}

	if (m_iHp <= 0)
		m_eCurState = MS_DEATH;


		m_bIsActive = true;

}

void CHellBoss::Update(_float fTimeDelta)
{
	if (m_pTarget == nullptr)
		return;
	/*if (!m_bCheck)
	{
		m_pGameInstance->Add_Collider(CG_MONSTER, m_pColliderCom);
		return;
	}*/

	
	m_vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	Select_Pattern(fTimeDelta);



	__super::Update(fTimeDelta);



	static float fTimeAcc = 0.f; 
	fTimeAcc += fTimeDelta;

	static pair<_int,_int> tCurRange = { 0, 0 }; 
	auto SetAnim = [&](const string& key)
		{
			auto it = m_TextureRanges.find(key);
			if (it != m_TextureRanges.end())
			{
				tCurRange = it->second;
				m_iCurrentFrame = tCurRange.first;
				fTimeAcc = 0.f; 
			}
		};


	if (GetAsyncKeyState('1') & 0x8000) SetAnim("1_Idle");
	else if (GetAsyncKeyState('2') & 0x8000) SetAnim("2_Walk");
	else if (GetAsyncKeyState('3') & 0x8000) SetAnim("3_EyeBlast");
	else if (GetAsyncKeyState('4') & 0x8000) SetAnim("4_Shoot");
	else if (GetAsyncKeyState('5') & 0x8000) SetAnim("5_Morph");
	else if (GetAsyncKeyState('6') & 0x8000) SetAnim("6_Phase2_Idle");
	else if (GetAsyncKeyState('7') & 0x8000) SetAnim("7_Phase2_Walk");
	else if (GetAsyncKeyState('8') & 0x8000) SetAnim("8_Phase2_Charge");
	else if (GetAsyncKeyState('9') & 0x8000) SetAnim("9_Phase2_Spin");
	else if (GetAsyncKeyState('0') & 0x8000) SetAnim("0_Phase2_Shoot");
	else if (GetAsyncKeyState('T') & 0x8000) SetAnim("T_Phase2_End");
	else if (GetAsyncKeyState('Y') & 0x8000) SetAnim("Y_ArmCut");
	else if (GetAsyncKeyState('U') & 0x8000) SetAnim("U_ArmCut_Idle");
	else if (GetAsyncKeyState('I') & 0x8000) SetAnim("I_ArmCut_Walk");
	else if (GetAsyncKeyState('P') & 0x8000) SetAnim("P_ArmCut_Attack");
	else if (GetAsyncKeyState('G') & 0x8000) SetAnim("G_Phase3_Idle");
	else if (GetAsyncKeyState('H') & 0x8000) SetAnim("H_Phase3_Search");
	else if (GetAsyncKeyState('J') & 0x8000) SetAnim("J_Phase3_TripleEye");
	else if (GetAsyncKeyState('K') & 0x8000) SetAnim("K_Phase3_Nova");
	else if (GetAsyncKeyState('L') & 0x8000) SetAnim("L_Phase3_Spawn");
	else if (GetAsyncKeyState('O') & 0x8000) SetAnim("O_Phase4_Idle");
	else if (GetAsyncKeyState('M') & 0x8000) SetAnim("M_Phase4_Death");


	if (fTimeAcc >= 0.05f)  // 20FPS 기준 (0.05초에 한 프레임)
	{
		fTimeAcc = 0.f;
		if (m_iCurrentFrame < tCurRange.second)
			++m_iCurrentFrame;
		else
			m_iCurrentFrame = tCurRange.first; 
	}


}

void CHellBoss::Late_Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;


	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
			return;

}

HRESULT CHellBoss::Render()
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


HRESULT CHellBoss::On_Collision(CCollisionObject* other)
{
	if (nullptr == m_pColliderCom)
		return E_FAIL;
	if (nullptr == other)
		return S_OK;
	
	// 안바뀌면 충돌 안일어남
	if (other->Get_Type() == CG_END)
		return S_OK;

	
	_float3 fPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vMtv = m_pColliderCom->Get_MTV();
	_float3 vMove = { vMtv.x, 0.f, vMtv.z };

	switch (other->Get_Type())
	{
	case CG_PLAYER:

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
		//m_pTransformCom->Go_Backward(fTimeDelta);
	

		if (m_eCurState != MS_ATTACK)
		{
			Take_Damage(other);
			
			
		}
		else
		{
			m_iAp *= 3;
			Take_Damage(other);
			m_iAp /= 3;
		}
		
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vCurPos);
		break;

	case CG_WEAPON:

		
		m_eCurState = MS_HIT;

		break;

	case CG_MONSTER:
		m_vNextPos += vMove * 0.2f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vNextPos);

		break;
	case CG_STRUCTURE_WALL:
		m_vNextPos += vMove;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vNextPos);
		break;
	case CG_DOOR:
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vCurPos);

		break;
	default:
		break;
	}

	

	return S_OK;
}

void CHellBoss::Select_Pattern(_float fTimeDelta)
{
	if (m_eCurState == MS_DEATH)
		return;

	_float3 vDist;
	vDist = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - static_cast<CPlayer*>(m_pTarget)->Get_TransForm()->Get_State(CTransform::STATE_POSITION);


	switch (m_eCurState)
	{
	case 1:
	   
		break;
	default:
		break;
	}


	
}



HRESULT CHellBoss::SetUp_RenderState()
{
	

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)

	return S_OK;
}

HRESULT CHellBoss::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CHellBoss::Ready_Components()
{




	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_HONG, TEXT("Prototype_Component_Texture_HellBoss"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 10.f, D3DXToRadian(90.f) };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;


	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 0.5f,0.5f,0.5f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	_float3 fPos = { 10.f, 0.5f, 10.f };

	/* For.Com_Collider */
	CCollider_Cube::COL_CUBE_DESC	Collider_AttackDesc = {};
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 1.f, 1.f, 1.f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Cube"),
		TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackCollider), &Collider_AttackDesc)))
		return E_FAIL;

	return S_OK;
}

CHellBoss* CHellBoss::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHellBoss* pInstance = new CHellBoss(pGraphic_Device);

 	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHellBoss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHellBoss::Clone(void* pArg)
{
	CHellBoss* pInstance = new CHellBoss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CHellBoss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellBoss::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pAttackCollider);
	Safe_Release(m_pTarget);
}



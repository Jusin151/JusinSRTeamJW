#include "Tree.h"
#include "GameInstance.h"
#include "Gib_Effect.h"

CTree::CTree(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CDeco_Base(pGraphic_Device)
{
}

CTree::CTree(const CTree& Prototype) 
	:CDeco_Base(Prototype)
{
}

HRESULT CTree::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTree::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(2.f, 4.f, 2.f);
	m_pColliderCom->Set_Scale(_float3(2.f, 4.f, 2.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(8.f, 0.46f, -25.f));

	m_eType = CG_OBJECT;

	m_iCurrentFrame = 0;

	m_iHp = 45;

	m_iCurHp = m_iHp;
	m_iPreHp = m_iHp;

	return S_OK;
}

HRESULT CTree::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

void CTree::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);


}

void CTree::Update(_float fTimeDelta)
{
	if (!m_bInit)
	{
		m_vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_vNextPos = m_vCurPos;
		m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pColliderCom->Get_Scale());
		Calc_Position();
		m_bInit = true;
	}

	Look_Player();


	


	if (m_eCurState != DS_DEATH)
	{
		m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pColliderCom->Get_Scale());
		
	}

	m_pGameInstance->Add_Collider(CG_OBJECT, m_pColliderCom);
}

void CTree::Late_Update(_float fTimeDelta)
{
	Select_State();

	Calc_Position();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vCurPos);


	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CTree::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(m_iHitCount)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	SetUp_RenderState();
	if (FAILED(m_pShaderCom->Bind_Transform()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, m_iHitCount)))
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

HRESULT CTree::On_Collision(CCollisionObject* other)
{
	if (nullptr == m_pColliderCom)
		return E_FAIL;

	if (nullptr == other)
		return S_OK;

	// 안바뀌면 충돌 안일어남
	if (other->Get_Type() == CG_END)
		return S_OK;

	switch (other->Get_Type())
	{
	case CG_PLAYER:

		break;

	case CG_WEAPON:
		m_eCurState = DS_HIT;
		

		break;

	case CG_MONSTER:

		break;

	case CG_PLAYER_PROJECTILE_SPHERE:
		m_eCurState = DS_HIT;
		

		break;

	case CG_PLAYER_PROJECTILE_CUBE:
		m_eCurState = DS_HIT;
		

		break;

	default:
		break;
	}

	return S_OK;
}

HRESULT CTree::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 100); // 기준값 설정 (0~255)
	_float2 ScaleFactor = { 1.0f, 1.0f };
	_float2 Offset = { 0.f, 0.f };
	m_pShaderCom->Set_UVScaleFactor(&ScaleFactor);
	m_pShaderCom->Set_UVOffsetFactor(&Offset);
	return S_OK;
}

HRESULT CTree::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

void CTree::Select_State()
{
	m_iCurHp = m_iHp;
	

 	if (m_eCurState == DS_HIT)
	{

		if (m_iCurHp != m_iPreHp)
		{
			++m_iHitCount;
			m_iPreHp = m_iCurHp;
		}
		else
		{
			m_eCurState = DS_IDLE;
			return;
		}
			

		m_pColliderCom->Set_Scale(_float3(2.f, floor(4.f / m_iHitCount)  , 2.f));
		m_pTransformCom->Set_Scale(2.f, floor(4.f / m_iHitCount)  , 2.f);


		m_eCurState = DS_IDLE;

		// gib �߰�..

		float offsetRangeX = 1.f, offsetRangeY = 1.f;

		CGib_Effect::HIT_DESC hitDesc;
		hitDesc.vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		hitDesc.vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
		hitDesc.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		hitDesc.vScale = { 1.5f, 1.f, 1.5f };
		hitDesc.type = 3;

		hitDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pGameInstance->Add_GameObject(
			LEVEL_STATIC,
			TEXT("Prototype_GameObject_Gib_Effect"),
			LEVEL_STATIC,
			TEXT("Layer_Gib_Effect"),
			&hitDesc);
	}

	if (m_iHitCount >= 3)
	{
		m_eCurState = DS_DEATH;
		m_iHitCount = 3;
	}
		
}

CTree* CTree::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTree* pInstance = new CTree(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTree::Clone(void* pArg)
{
	CTree* pInstance = new CTree(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CTree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTree::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTarget);
}

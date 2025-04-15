#include "BombBox.h"
#include "GameInstance.h"
#include "Effects.h"

CBombBox::CBombBox(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CDeco_Base(pGraphic_Device)
{
}

CBombBox::CBombBox(const CBombBox& Prototype)
	:CDeco_Base(Prototype)
{
}

HRESULT CBombBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBombBox::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
	m_pColliderCom->Set_Scale(_float3(1.f, 1.f, 1.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(-15.f, 0.46f, -32.f));

	m_eType = CG_OBJECT;

	m_iCurrentFrame = 0;

	m_iAp = 50;

	return S_OK;
}

HRESULT CBombBox::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BombBox"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider::COL_DESC	ColliderDesc = {};
	ColliderDesc.pOwner = this;
	// 이걸로 콜라이더 크기 설정
	ColliderDesc.fScale = { 7.f,7.f,7.f };
	// 오브젝트와 상대적인 거리 설정
	ColliderDesc.fLocalPos = { 0.f, 0.f, 0.f };


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

void CBombBox::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CBombBox::Update(_float fTimeDelta)
{
	if (!m_bInit)
	{
		m_vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_vNextPos = m_vCurPos;
		m_pColliderCom->Update_Collider(TEXT("Com_Transform"), m_pColliderCom->Get_Scale());
		m_pAttackColliderCom->Update_Collider(TEXT("Com_Transform"), m_pAttackColliderCom->Get_Scale());
		Calc_Position();
		m_bInit = true;
	}

	Look_Player();


	if (m_eCurState == DS_DEATH)
		return;

	if (m_bExplosion)
	{
		m_eType = CG_BOMB;
		m_pGameInstance->Add_Collider(CG_BOMB, m_pAttackColliderCom);
		m_eCurState = DS_DEATH;
	}
	else if (m_eCurState == DS_IDLE || m_eCurState != m_ePreState)
	{
		m_pGameInstance->Add_Collider(CG_OBJECT, m_pColliderCom);
	}

}

void CBombBox::Late_Update(_float fTimeDelta)
{
	Select_State();

	Calc_Position();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vCurPos);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
		return;
}

HRESULT CBombBox::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	SetUp_RenderState();
	if (FAILED(m_pShaderCom->Bind_Transform()))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Texture(m_pTextureCom, m_iCurrentFrame)))
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

HRESULT CBombBox::On_Collision(CCollisionObject* other)
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
		if (m_bExplosion)
			Take_Damage(other);
		break;

	case CG_WEAPON:
		m_eCurState = DS_HIT;


		break;

	case CG_MONSTER:
		if (m_bExplosion)
			Take_Damage(other);
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

HRESULT CBombBox::SetUp_RenderState()
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

HRESULT CBombBox::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

void CBombBox::Select_State()
{
	if (m_eCurState == DS_DEATH)
	{
		m_ePreState = DS_DEATH;
		return;
	}

	if (m_eCurState == DS_HIT)
	{

		m_pTransformCom->Set_Scale(1.f, 0.5f, 1.f);
		m_pColliderCom->Set_Scale(_float3(1.f, 0.5f, 1.f));
		m_iCurrentFrame = 1;
		m_bExplosion = true;

		// 폭발 이펙트 생성
		Explosion();
		

	}

	

}

void CBombBox::Explosion()
{
	// 폭발 이펙트 생성

	CEffect_Base::EFFECT_DESC effectDesc;
	effectDesc.vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	effectDesc.vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	effectDesc.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	effectDesc.vScale = { 5.f, 5.f, 5.f };
	

	effectDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	effectDesc.vPos += {0.f, 2.f, 0.f};
	m_pGameInstance->Add_GameObject(
		LEVEL_STATIC,
		TEXT("Prototype_GameObject_Explosion_Effect"),
		LEVEL_STATIC,
		TEXT("Layer_Explosion_Effect"),
		&effectDesc);


	float offsetRangeX = 1.f, offsetRangeY = 1.f;


	CEffect_Base::EFFECT_DESC effectDesc2;
	effectDesc2.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	effectDesc2.vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	effectDesc2.vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	effectDesc2.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	effectDesc2.vScale = { 1.f, 1.f, 1.f };

	m_pGameInstance->Add_GameObject(
		LEVEL_STATIC,
		TEXT("Prototype_GameObject_Fire_Effect"),
		LEVEL_STATIC,
		TEXT("Layer_Fire_Effect"),
		&effectDesc2);
}

CBombBox* CBombBox::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBombBox* pInstance = new CBombBox(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBombBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBombBox::Clone(void* pArg)
{
	CBombBox* pInstance = new CBombBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBombBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBombBox::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pAttackColliderCom);
	Safe_Release(m_pTarget);
}

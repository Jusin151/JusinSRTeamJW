#include "Bullet_Base.h"
#include "Monster_Base.h"
#include "VIBuffer_Rect.h"
#include "Texture.h"
#include "Collider_Cube.h"
#include "Transform.h"
#include "Player.h"
#include "GameInstance.h"
#include "Magic_Effect.h"

CBullet_Base::CBullet_Base(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CCollisionObject(pGraphic_Device)
{
}

CBullet_Base::CBullet_Base(const CBullet_Base& Prototype)
	:CCollisionObject(Prototype)
{
}

HRESULT CBullet_Base::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBullet_Base::Initialize(void* pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_Player_Transform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Instance()->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	Player_RIght = m_Player_Transform->Get_State(CTransform::STATE_RIGHT);
	Player_Up = m_Player_Transform->Get_State(CTransform::STATE_UP);
	Player_Look = m_Player_Transform->Get_State(CTransform::STATE_LOOK);
	Player_Pos = m_Player_Transform->Get_State(CTransform::STATE_POSITION);


	/*Player_Pos += Player_Look * 0.8f;
	Player_Pos += Player_RIght * 0.1f;
	Player_Pos.y -= 0.5f;*/

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, Player_RIght);
	m_pTransformCom->Set_State(CTransform::STATE_UP, Player_Up);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, Player_Look);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Player_Pos);
	m_pTransformCom->Set_Scale(0.6f, 0.6f, 0.6f);


	m_fSpeed = 5.f;
	return S_OK;
}

void CBullet_Base::Priority_Update(_float fTimeDelta)
{

}

void CBullet_Base::Update(_float fTimeDelta)
{

	m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed);

	m_fElapsedTime += fTimeDelta;
	if (m_fElapsedTime >= 0.02f)
	{
		m_fElapsedTime = 0.0f;
		if (m_iCurrentFrame < 6)
		{
			m_iCurrentFrame = (m_iCurrentFrame + 1) % 7; // 팀장님이 알려준
		}
		else
		{
			m_iCurrentFrame = 6;
		}
	}
}



void CBullet_Base::Late_Update(_float fTimeDelta)
{

	if (m_pGameInstance->IsAABBInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Compute_Scaled()))
	{

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this)))
		return;
	}
}


HRESULT CBullet_Base::Render()
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

HRESULT CBullet_Base::CreateParticle(DWORD dwInit, DWORD dwFade)
{
	CMagic_Effect::HIT_DESC hitDesc = {};
	hitDesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	hitDesc.vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	hitDesc.vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	hitDesc.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	hitDesc.vScale = { 1.0f, 1.0f, 1.0f };
	hitDesc.dwInitialColor = dwInit;
	hitDesc.dwFadeColor = dwFade;
	if (FAILED(m_pGameInstance->Add_GameObject(
		LEVEL_STATIC,
		TEXT("Prototype_GameObject_Magic_Effect"),
		LEVEL_STATIC,
		TEXT("Layer_Magic_Effect"), &hitDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CBullet_Base::On_Collision(CCollisionObject* other)
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

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);
		//m_pTransformCom->Go_Backward(fTimeDelta);
		break;

	case CG_WEAPON:



		break;
	default:
		break;
	}



	return S_OK;
}


void CBullet_Base::Attack_Melee()
{
	m_pAttackCollider->Update_Collider(TEXT("Com_Transform"), m_pAttackCollider->Get_Scale());

	m_pGameInstance->Add_Collider(CG_MONSTER_PROJECTILE_CUBE, m_pAttackCollider);
}

HRESULT CBullet_Base::SetUp_RenderState()
{
	// 일단 추가해보기

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)

	return S_OK;
}

HRESULT CBullet_Base::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CBullet_Base::Ready_Components()
{


	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBufferm"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	_float3 fPos = { 10.f, 0.5f, 10.f };



	

	return S_OK;
}

CBullet_Base* CBullet_Base::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBullet_Base* pInstance = new CBullet_Base(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBullet_Base");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBullet_Base::Clone(void* pArg)
{
	CBullet_Base* pInstance = new CBullet_Base(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CBullet_Base");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBullet_Base::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pAttackCollider);
}

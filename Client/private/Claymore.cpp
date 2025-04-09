#include "Claymore.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Item_Manager.h"
#include "Image_Manager.h"
#include "Effects.h"



CClaymore::CClaymore(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMelee_Weapon(pGraphic_Device)
{
}

CClaymore::CClaymore(const CClaymore& Prototype)
	: CMelee_Weapon(Prototype),
	m_Claymore_INFO{Prototype.m_Claymore_INFO }
{
}

HRESULT CClaymore::Initialize_Prototype()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

HRESULT CClaymore::Initialize(void* pArg)
{

	if (pArg != nullptr)
	{
		m_Claymore_INFO = *reinterpret_cast<Weapon_DESC*>(pArg);
	}
	else
		return E_FAIL;

	if (FAILED(CMelee_Weapon::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pColliderCom->Set_Scale(_float3(2.f, 1.f, 2.f));

	m_pColTransformCom->Set_Scale(2.f, 1.f, 2.f);


	m_pTransformCom->Set_Scale(m_Claymore_INFO.vSize.x, m_Claymore_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Claymore_INFO.vPos.x, m_Claymore_INFO.vPos.y, 0.f));

	m_vInitialPos = m_Claymore_INFO.vPos;
	CItem_Manager::GetInstance()->Add_Weapon(L"Claymore",this);
	


	m_eType = CG_WEAPON;

	return S_OK;
}





void CClaymore::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CClaymore::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


 
}

void CClaymore::Late_Update(_float fTimeDelta)
{
	if (!IsActive())
		return;

	

	__super::Late_Update(fTimeDelta);
}

HRESULT CClaymore::Render()
{
	
	__super::Render();

	m_pGameInstance->Render_Font_Size(L"MainFont", TEXT("클레이모어 공격력:") + to_wstring(m_iAp),
		_float2(350.f, -300.f), _float2(8.f, 0.f), _float3(1.f, 1.f, 0.f));

	return S_OK;

}

HRESULT CClaymore::On_Collision(CCollisionObject* other)
{
	if (nullptr == m_pColliderCom)
		return E_FAIL;

	if (!m_bIsAnimating)
		return S_OK;
	

	if (nullptr == other)
		return S_OK;

	// 안바뀌면 충돌 안일어남
	if (other->Get_Type() == CG_END)
		return S_OK;

	
	

	_float3 vMtv = m_pColliderCom->Get_MTV();
	_float3 vMove = { vMtv.x, 0.f, vMtv.z };


	switch (other->Get_Type()) //여기서 디버깅 잡히나?
	{
	case CG_MONSTER:

		// 나중에 공격력 만들어서 추가하는 식으로
			Take_Damage(other);
			m_bAttack = true;
		break;
	case CG_STRUCTURE_WALL:
		m_bAttack = true;

	default:
		break;
	}

	if (other->Get_Type() == CG_STRUCTURE_WALL && true == m_bAttack)
	{
		// 여기서  이펙트 생성


		
	}

	

	return S_OK;
}

HRESULT CClaymore::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Claymore"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CClaymore* CClaymore::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CClaymore* pInstance = new CClaymore(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{

		MSG_BOX("클레이모어 UI 원본 생성 실패 ");

		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CClaymore::Clone(void* pArg)
{
	CClaymore* pInstace = new CClaymore(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("클레이모어 UI 복제 실패");

		Safe_Release(pInstace);
	}

	return pInstace;
}

void CClaymore::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
void CClaymore::Attack(_float fTimeDelta)
{
	if (m_bIsAnimating) return;

		m_bIsAnimating = true;
		m_bAttack = false;
		m_iCurrentFrame = 0;
		m_fElapsedTime = 0.0f;
		m_iLastFrame = 13;
}

void CClaymore::Attack_WeaponSpecific(_float fTimeDelta)
{


}

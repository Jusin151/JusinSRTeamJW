#include "Claymore.h"
#include "GameInstance.h"
#include "CUI_Manager.h"
#include "Item_Manager.h"


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
	if (FAILED(CMelee_Weapon::Ready_Components()))
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

	m_pColliderCom->Set_Scale(_float3(3.f, 0.5f, 3.f));

	m_pColTransformCom->Set_Scale(1.f, 1.f, 1.f);
	m_pColTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(0.f, 0.f, 0.5f));


	m_pTransformCom->Set_Scale(m_Claymore_INFO.vSize.x, m_Claymore_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Claymore_INFO.vPos.x, m_Claymore_INFO.vPos.y, 0.f));

	m_vInitialPos = m_Claymore_INFO.vPos;
	CItem_Manager::GetInstance()->Add_Weapon(L"Claymore",this);



	m_eType = CG_WEAPON;
	m_iAp = 15;

	return S_OK;
}

void CClaymore::Priority_Update(_float fTimeDelta)
{
}

void CClaymore::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

	Attack(fTimeDelta);
}

void CClaymore::Late_Update(_float fTimeDelta)
{
	if (!IsActive())
		return;

	

	__super::Late_Update(fTimeDelta);
}

HRESULT CClaymore::Render()
{
	
	return __super::Render();
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

	

	switch (other->Get_Type()) //여기서 디버깅 잡히나?
	{
	case CG_MONSTER:

		// 나중에 공격력 만들어서 추가하는 식으로
			Take_Damage(other);
			m_bAttack = true;
		break;

	default:
		break;
	}

	

	return S_OK;
}

HRESULT CClaymore::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Claymore"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_Orth"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
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
	if (!m_bIsAnimating && (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		m_bIsAnimating = true;
		m_bAttack = false;
		m_iCurrentFrame = 0;
		m_fElapsedTime = 0.0f;
	}

	if (m_bIsAnimating)
	{
		m_fElapsedTime += fTimeDelta;

		if (m_fElapsedTime >= 0.02f)
		{
			m_fElapsedTime = 0.0f;

			if (m_iCurrentFrame < 13)
			{
				m_iCurrentFrame++;
			}
			else
			{
				m_bIsAnimating = false;
			}
		}
	}
}
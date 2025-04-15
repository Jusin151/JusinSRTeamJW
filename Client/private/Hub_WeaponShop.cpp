#include "Hub_WeaponShop.h"
#include "GameInstance.h"
#include "Player.h"  
#include "UI_WeaponShop_UI.h"
#include "UI_Manager.h"

CHub_WeaponShop::CHub_WeaponShop(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CShop(pGraphic_Device)
{
}

CHub_WeaponShop::CHub_WeaponShop(const CHub_WeaponShop& Prototype)
	: CShop(Prototype)
{
}

HRESULT CHub_WeaponShop::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHub_WeaponShop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(-6.5f, 0.6f, -2.2f));

	m_pTransformCom->Set_Scale(1.5f, 1.5f, 2.f);

	// m_pUI_WeaponShop = static_cast<CUI_WeaponShop_UI*>(CUI_Manager::GetInstance()->GetUI(L"Weapon_Shop_UI"));

	if (auto pWeaponShopUI = dynamic_cast<CObserver*>(CUI_Manager::GetInstance()->GetUI(L"Weapon_Shop_UI")))
		Add_Observer(pWeaponShopUI);

	return S_OK;
}


void CHub_WeaponShop::Priority_Update(_float fTimeDelta)
{
	m_bIsActive = true;
}

void CHub_WeaponShop::Update(_float fTimeDelta)
{

	__super::Update(fTimeDelta);

	if (m_bIsOpen)
	{
		Open_Shop();
	}
	else
	{
		Close_Shop();
	}
}

void CHub_WeaponShop::Late_Update(_float fTimeDelta)
{

	__super::Late_Update(fTimeDelta);
}

HRESULT CHub_WeaponShop::SetUp_RenderState()
{
	// 일단 추가해보기

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)
	_float2 ScaleFactor = { 1.0f, 1.0f };
	_float2 Offset = { 0.f, 0.f };
	m_pShaderCom->Set_UVScaleFactor(&ScaleFactor);
	m_pShaderCom->Set_UVOffsetFactor(&Offset);

	return S_OK;
}

HRESULT CHub_WeaponShop::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CHub_WeaponShop::Render()
{

	__super::Render();
	return S_OK;
}

HRESULT CHub_WeaponShop::Ready_ShopItems()
{
	return S_OK;
}


PurchaseStatus CHub_WeaponShop::Purchase_Item(const _wstring& stItemName, _uint iPrice)
{
	if (!m_bIsOpen) return PurchaseStatus::ShopClosed;

	return Can_Purchase(stItemName, iPrice);
}

PurchaseStatus CHub_WeaponShop::Can_Purchase(const _wstring& stItemName, _uint iPrice)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pPlayer);
	if (pPlayer)
	{
		if (pPlayer->Has_Item(stItemName)) return PurchaseStatus::AlreadyOwned;

		if (pPlayer->Get_PlayerInfo().iSkillpoint >= iPrice)
		{
			pPlayer->Use_SkillPoint(iPrice);
			_uint iSkillPoint = pPlayer->Get_PlayerInfo().iSkillpoint;
			Notify(&iSkillPoint, L"Open");
			pPlayer->Add_Weapon(stItemName);
			return PurchaseStatus::Success;
		}
		else
		{
			return PurchaseStatus::NotEnoughSkillPoint;
		}
	}

	return PurchaseStatus::ShopClosed;
}


HRESULT CHub_WeaponShop::Open_Shop()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pPlayer);
	if (pPlayer)
	{
		_uint iSkillPoint = pPlayer->Get_PlayerInfo().iSkillpoint;
		Notify(&iSkillPoint, L"Open");
	}

	return S_OK;
}//뭘봐 ㅋ

HRESULT CHub_WeaponShop::Close_Shop()
{
	// 이미 닫혀있다면 무시

	Notify(nullptr, L"Close");


	return S_OK;
}


HRESULT CHub_WeaponShop::Ready_Components()
{

	if (FAILED(__super::Add_Component(LEVEL_HUB, TEXT("Prototype_Component_Texture_Weapon_Shop"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CHub_WeaponShop* CHub_WeaponShop::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHub_WeaponShop* pInstance = new CHub_WeaponShop(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHub_WeaponShop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHub_WeaponShop::Clone(void* pArg)
{
	CHub_WeaponShop* pInstance = new CHub_WeaponShop(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHub_WeaponShop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHub_WeaponShop::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);

}

#include "Inventory.h"
#include "GameInstance.h"
#include "Item_Manager.h"
#include "UI_Manager.h"


CInventory::CInventory(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CInventory::CInventory(const CInventory& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CInventory::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CInventory::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_Inven_INFO.vSize = { 918.f,207.f };
	m_Inven_INFO.vPos = { 0.f,170.f };
	m_pTransformCom->Set_Scale(m_Inven_INFO.vSize.x, m_Inven_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Inven_INFO.vPos.x, m_Inven_INFO.vPos.y, 0.f));
	 


	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Inven_UI"),
		LEVEL_GAMEPLAY, TEXT("Layer_Inven_UI"))))
		return E_FAIL; 

	//if (m_pInven_UI = dynamic_cast<CInven_UI*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, L"Layer_Inven_UI")));
	//else
	//	return E_FAIL;

//	m_vecpItem.resize(7); // 7번까지만 할래

	bFirstUpdate = true;

 	if (auto pInvenUI = dynamic_cast<CObserver*>(CUI_Manager::GetInstance()->GetUI(L"Inven_UI")))
		Add_Observer(pInvenUI);

	//CItem_Manager::GetInstance()->Add_Inven(this);

	return S_OK;
} 


void CInventory::Priority_Update(_float fTimeDelta)
{
}

void CInventory::Update(_float fTimeDelta)
{
	if (m_bKeyPressed)
		m_fNoInputAccTime = 0.f;
	else
		m_fNoInputAccTime += fTimeDelta;

	if (m_fNoInputAccTime >= 2.f)
	{
		Notify(nullptr, L"Close");
		return;
	}
	if (m_bKeyPressed)
		Notify(nullptr, L"Open");

	//m_pInven_UI->Inven_OnOff(true);

	m_bKeyPressed = false;
}

void CInventory::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
	if (!m_bFrist_off_Item)
	{
		CItem_Manager::GetInstance()->Weapon_UnEquip();
		m_bFrist_off_Item = true;
	}
}

HRESULT CInventory::Render(){return S_OK;}

HRESULT CInventory::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;
	return S_OK;
}
CWeapon_Base* CInventory::Equip(_uint type)
{

	//m_bKeyPressed = false;

		m_pItem = Weapon_Equip(type-1);
		m_bKeyPressed = true;
	

	return m_pItem;
}



CInventory* CInventory::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CInventory* pInstance = new CInventory(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("인벤 UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInventory::Clone(void* pArg)
{
	CInventory* pInstace = new CInventory(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("인벤 UI 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CInventory::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
//if (!m_pWeapon) return;
// 
//if (CRanged_Weapon* pRangeWeapon = dynamic_cast<CRanged_Weapon*>(m_pWeapon))
//{
//    pRangeWeapon->Add_Ammo(iAmmo);
//}
#include "Hub_PointShop.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "UI_Point_Shop.h"


CHub_PointShop::CHub_PointShop(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CShop(pGraphic_Device)
{
}

CHub_PointShop::CHub_PointShop(const CHub_PointShop& Prototype)
	: CShop(Prototype)
{
}

HRESULT CHub_PointShop::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CHub_PointShop::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(1.5f, 0.6f, 0.5f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(-5.f, 0.6f, 8.3f));

	m_pTransformCom->Set_Scale(1.5f, 1.5f, 2.f);


	m_pUI_Point_Hub = static_cast<CUI_Point_Shop*>(CUI_Manager::GetInstance()->GetUI(L"Point_Shop_UI"));

	/*   if (m_pUI_Point_Hub == nullptr)
		   return E_FAIL;*/

	m_pUI_Point_Hub = static_cast<CUI_Point_Shop*>(CUI_Manager::GetInstance()->GetUI(L"Point_Shop_UI"));
	if (m_pUI_Point_Hub)
	{
		m_pUI_Point_Hub->Set_Shop(this);
	}

	
	return S_OK;
}


void CHub_PointShop::Priority_Update(_float fTimeDelta)
{
	m_bIsActive = true;
}

void CHub_PointShop::Update(_float fTimeDelta)
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

void CHub_PointShop::Late_Update(_float fTimeDelta)
{

	__super::Late_Update(fTimeDelta);
}

HRESULT CHub_PointShop::Render()
{


	if (FAILED(m_pTextureCom->Bind_Resource(0)))
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

HRESULT CHub_PointShop::Ready_ShopItems()
{


	return S_OK;
}

HRESULT CHub_PointShop::Open_Shop()
{
	if (m_pUI_Point_Hub == nullptr) return E_FAIL;
	// 이미 열려있다면 무시
	if (m_pUI_Point_Hub->IsActive())
		return S_OK;


	m_pUI_Point_Hub->SetActive(true); // 보이게 설정
	m_pUI_Point_Hub->Button_Set_Active(true);
	m_pUI_Point_Hub->m_bOnUI = true;


	// 상점 아이템 새로고침
	Refresh_Shop_Items();

	return S_OK;
}//뭘봐 ㅋ

HRESULT CHub_PointShop::Close_Shop()
{
	if (m_pUI_Point_Hub == nullptr) return E_FAIL;
	if (!m_pUI_Point_Hub->IsActive())
		return S_OK;


	m_pUI_Point_Hub->SetActive(false);
	m_pUI_Point_Hub->Button_Set_Active(false);
	m_pUI_Point_Hub->m_bOnUI = false;



	return S_OK;
}

HRESULT CHub_PointShop::Purchase_Item(const _uint iItemID, const _uint iCount)
{
	// 상점이 닫혀있다면 구매 실패
	if (!m_bIsOpen)
		return E_FAIL;

	// 구매 가능 여부 확인
	if (!Can_Purchase(iItemID, iCount))
		return E_FAIL;
	return S_OK;
}

HRESULT CHub_PointShop::Sell_Item(const _uint iItemID, const _uint iCount)
{
	return S_OK;
}

void CHub_PointShop::Refresh_Shop_Items()
{


}
HRESULT CHub_PointShop::SetUp_RenderState()
{
	// 일단 추가해보기

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); // 알파 값이 기준보다 크면 픽셀 렌더링
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200); // 기준값 설정 (0~255)

	return S_OK;
}

HRESULT CHub_PointShop::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

_bool CHub_PointShop::Can_Purchase(_uint iItemID, _uint iCount)
{
	// 상점이 닫혀있으면 구매 불가
	if (!m_bIsOpen)
		return false;

	return true;
}


HRESULT CHub_PointShop::Ready_Components()
{

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Point_Shop"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

CHub_PointShop* CHub_PointShop::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHub_PointShop* pInstance = new CHub_PointShop(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHub_PointShop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHub_PointShop::Clone(void* pArg)
{
	CHub_PointShop* pInstance = new CHub_PointShop(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHub_PointShop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHub_PointShop::Free()
{
	__super::Free();

}

void CHub_PointShop::Buy_Stat(_uint iStatIndex)
{
	if (m_pPlayer == nullptr)
		return;

	switch (iStatIndex)
	{
	case STR:
		if (CPlayer* m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"))))
		{
			m_pPlayer->Add_Strength(1);
		}
		break;
	case MAXHP:
		if (CPlayer* m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"))))
		{
			m_pPlayer->Add_MaxHP(5);
		}
		break;
	case SPRIT:
		if (CPlayer* m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"))))
		{
			m_pPlayer->Add_Sprit(1);
		}
		break;
	case CAPACITY:
		if (CPlayer* m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Player"))))
		{
			m_pPlayer->Add_Capacity(1);
		}
		break;
	}
}


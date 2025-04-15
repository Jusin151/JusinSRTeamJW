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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(1.5f, 0.6f, 0.5f));

	m_pTransformCom->Set_Scale(1.5f, 1.5f, 2.f);

	if (auto pPointShopUI = dynamic_cast<CObserver*>(CUI_Manager::GetInstance()->GetUI(L"Point_Shop_UI")))
 		Add_Observer(pPointShopUI);

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
	__super::Render();
	return S_OK;
}

HRESULT CHub_PointShop::Ready_ShopItems()
{


	return S_OK;
}

HRESULT CHub_PointShop::Open_Shop()
{
	Notify(nullptr, L"Open");

	return S_OK;
}

HRESULT CHub_PointShop::Close_Shop()
{
	Notify(nullptr, L"Close");

	return S_OK;
}

HRESULT CHub_PointShop::SetUp_RenderState()
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

HRESULT CHub_PointShop::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}


void CHub_PointShop::Buy_Stat(_int index)
{
	Notify(&index, L"StatBuy");  // 성공했을 때만 UI 갱신
}
void CHub_PointShop::Buy_Skill(_int index)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	if (!pPlayer)
		return;

	switch (index)
	{
	case 6:
		if (pPlayer->Get_PlayerInfo().iSkillpoint >= 5)
		{
			pPlayer->Use_SkillPoint(5);
			pPlayer->Set_DoubleAmmoGain(true);
			MessageBox(nullptr, L"탄약획득량이 2배 증가했습니다!", L"더 많이!", MB_OK);
		}
		break;

	case 11:
		if (pPlayer->Get_PlayerInfo().iSkillpoint >= 3)
		{
			pPlayer->Use_SkillPoint(3);
			pPlayer->Set_DoubleSpeedGain();
			MessageBox(nullptr, L"스피드가 2배로 증가했습니다!", L"더욱 빠르게!", MB_OK);
		}
		break;

		// 여기에 더 추가 가능
	}
}

void CHub_PointShop::Buy_Skill(const _wstring& stSkillName)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Player")));
	if (!pPlayer)
		return;

	auto pUI_Event = static_cast<CUI_Event*>(CUI_Manager::GetInstance()->GetUI(TEXT("UI_Event")));
	CUI_Event::EVENT_RENDER_TEXT vRenderText;

	if (stSkillName == TEXT("Skill_6"))
	{
		pPlayer->Set_DoubleAmmoGain(true);
		vRenderText.stText = TEXT("탄약획득량이 2배 증가했습니다!\n               더 많이!");
	}
	else if (stSkillName == TEXT("Skill_11"))
	{
		pPlayer->Set_DoubleSpeedGain();
		vRenderText.stText = TEXT("스피드가 2배로 증가했습니다!\n          더욱 빠르게!");
	}


	if (pUI_Event)
	{
		vRenderText.vPos = _float2(-100.f, -100.f);
		vRenderText.vFontSize = _float2(10.f, 30.f);
		vRenderText.vColor = _float3(1.f, 0.4f, 0.4f);
		vRenderText.fLifeTime = 0.5f;
		pUI_Event->Add_EventRender(vRenderText);
	}
}

PurchaseStatus CHub_PointShop::Purchase_Skill(const _wstring& stSkillName, _uint iPrice)
{
	if (!m_bIsOpen)
		return PurchaseStatus::ShopClosed;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pPlayer);
	if (!pPlayer)
		return PurchaseStatus::UnknownError;

	
	if (pPlayer->Has_Skill(stSkillName))
		return PurchaseStatus::AlreadyOwned;

	if (pPlayer->Get_PlayerInfo().iSkillpoint >= iPrice)
	{
		pPlayer->Use_SkillPoint(iPrice);
		Buy_Skill(stSkillName);
		pPlayer->Add_Skill(stSkillName);
		return PurchaseStatus::Success;
	}
	else
	{
		return PurchaseStatus::NotEnoughPoint;
	}
}

PurchaseStatus CHub_PointShop::Purchase_Stat(const _wstring& stStatName, _uint iPrice)
{
	if (!m_bIsOpen)
		return PurchaseStatus::ShopClosed;
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pPlayer);
	if (!pPlayer)
		return PurchaseStatus::UnknownError;

	if (pPlayer->Get_PlayerInfo().iStatpoint >= iPrice)
	{
		_uint iIndex = -1;
		if (stStatName == TEXT("Strength"))
		{
			iIndex = 0;
			pPlayer->Add_Strength(1);

		}
		else if (stStatName == TEXT("Life"))
		{
			iIndex = 1;
			pPlayer->Add_MaxHP(10);
		}
		else if (stStatName == TEXT("Sprit"))
		{
			iIndex = 2;
			pPlayer->Add_Sprit(1);
		}
		else if (stStatName == TEXT("Capacity"))
		{
			iIndex = 3;
			pPlayer->Add_Capacity(1);
		}
		else
			return PurchaseStatus::UnknownError;


		Notify(&iIndex, L"StatBuy");  // 성공했을 때만 UI 갱신
		return PurchaseStatus::Success;
	}
	else
	{
		return PurchaseStatus::NotEnoughPoint;
	}
}


HRESULT CHub_PointShop::Ready_Components()
{

	if (FAILED(__super::Add_Component(LEVEL_HUB, TEXT("Prototype_Component_Texture_Point_Shop"),
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






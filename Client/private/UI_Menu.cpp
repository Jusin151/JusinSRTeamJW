#include "UI_Menu.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Player.h"

CUI_Menu::CUI_Menu(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CUI_Menu::CUI_Menu(const CUI_Menu& Prototype)
	: CUI_Base(Prototype),
	m_INFO{ Prototype.m_INFO }
{
}

HRESULT CUI_Menu::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CUI_Menu::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))

		return E_FAIL;

	if (pArg != nullptr)
	{
		m_INFO = *reinterpret_cast<UI_Child_Desc*>(pArg);
		Set_Position(m_INFO.vPos);
		Set_Size(m_INFO.vSize);
		CUI_Manager::GetInstance()->AddUI(L"UI_Menu", this);
	}
	else
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_INFO.vSize.x, m_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_INFO.vPos.x, m_INFO.vPos.y, 0.f));
	return S_OK;
}

void CUI_Menu::Priority_Update(_float fTimeDelta)
{
}

void CUI_Menu::Update(_float fTimeDelta)
{
	if (GetAsyncKeyState('U') & 0x8000)
	{
		if (!m_bKeyPressed)
		{
			m_bIsVisible = !m_bIsVisible;
			m_bKeyPressed = true;
			//static_cast<CPlayer*>(m_pPlayer)->Taimu_S_to_pu();
		}
	}
	else
	{
		m_bKeyPressed = false; 
	}
}


void CUI_Menu::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}

HRESULT CUI_Menu::Render()
{
	if (!m_bIsVisible) // 이미지가 표시되지 않으면 렌더링 ㄴ
		return S_OK;

	return __super::Render();
}

HRESULT CUI_Menu::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Menu_Panel"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_Menu* CUI_Menu::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Menu* pInstance = new CUI_Menu(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("메뉴 UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CUI_Menu::Clone(void* pArg)
{
	CUI_Menu* pInstace = new CUI_Menu(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("메뉴 UI 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CUI_Menu::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pPlayer);
}

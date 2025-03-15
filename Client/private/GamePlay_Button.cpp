#include "GamePlay_Button.h"
#include "GameInstance.h"
#include "CUI_Manager.h"

CGamePlay_Button::CGamePlay_Button(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CGamePlay_Button::CGamePlay_Button(const CGamePlay_Button& Prototype)
	: CUI_Base(Prototype),
	m_GamePlay_Button_pTextureCom(Prototype.m_GamePlay_Button_pTextureCom),
	m_GamePlayer_Button_pVIBufferCom(Prototype.m_GamePlayer_Button_pVIBufferCom),
	m_GamePlayer_Button_pTransformCom{ Prototype.m_GamePlayer_Button_pTransformCom }
{
}

HRESULT CGamePlay_Button::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CGamePlay_Button::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		m_Button_INFO = *reinterpret_cast<GamePlayer_Button_Desc*>(pArg);
		Set_Position(m_Button_INFO.Button_Desc.vPos);
		Set_Size(m_Button_INFO.Button_Desc.vSize);
		CUI_Manager::GetInstance()->AddUI(m_Button_INFO.strUIName, this);
	}
	else
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_GamePlayer_Button_pTransformCom->Set_Scale(m_Button_INFO.Button_Desc.vSize.x, m_Button_INFO.Button_Desc.vSize.y, 1.f);
	m_GamePlayer_Button_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Button_INFO.Button_Desc.vPos.x, m_Button_INFO.Button_Desc.vPos.y, 0.f));
	return S_OK;
}

void CGamePlay_Button::Priority_Update(_float fTimeDelta)
{
}

void CGamePlay_Button::Update(_float fTimeDelta)
{
	switch (m_Button_INFO.Button_type)
	{
	case Episode:
		Episode_Display_Button();
		break;
	case Point_Shop:
		Point_Shop_Display_Button();
		break;
	case Spell_Shop:
		Spell_Shop_Display_Button();
		break;
	case Upgrade_Weapon:
		Upgrade_Weapon_Display_Button();
		break;
	default:
		break;
	}

}


void CGamePlay_Button::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}

HRESULT CGamePlay_Button::Render()
{
	D3DXMATRIX matOldView, matOldProj;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matOldProj);

	D3DXMATRIX matView;
	D3DXMatrixIdentity(&matView);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProj;
	D3DXMatrixOrthoLH(&matProj, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matProj);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	if (FAILED(m_GamePlayer_Button_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_GamePlay_Button_pTextureCom->Bind_Resource(Current_Image)))
			return E_FAIL;


	if (FAILED(m_GamePlayer_Button_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_GamePlayer_Button_pVIBufferCom->Render()))
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	return S_OK;
}

void CGamePlay_Button::Episode_Display_Button()
{
	if (m_Button_INFO.bLevel_Icon_Button_Flag == true)
	{
		if (true == isPick(g_hWnd))
			Current_Image = Level_ICon_Selected;
		else
			Current_Image = Level_ICon_Defaul;
	}
	if (m_Button_INFO.bLevel_01_Stage_Button_Flag == true)
	{
		if (true == isPick(g_hWnd))
			Current_Image = Stage_01_Default;
		else
			Current_Image = Stage_01_Default;
	}
	if (m_Button_INFO.bLevel_02_Stage_Button_Flag == true)
	{
		if (true == isPick(g_hWnd))
			Current_Image = Stage_02_Selected;
		else
			Current_Image = Stage_02_Default;
	}
	if (m_Button_INFO.bLevel_03_Stage_Button_Flag == true)
	{
		if (true == isPick(g_hWnd))
			Current_Image = Stage_03_Selected;
		else
			Current_Image = Stage_03_Default;
	}
	if (m_Button_INFO.bLevel_04_Stage_Button_Flag == true)
	{
		if (true == isPick(g_hWnd))
			Current_Image = Stage_04_Selected;
		else
			Current_Image = Stage_04_Default;
	}
	if (m_Button_INFO.bLevel_05_Stage_Button_Flag == true)
	{
		if (true == isPick(g_hWnd))
			Current_Image = Stage_05_Selected;
		else
			Current_Image = Stage_05_Default;
	}

}

void CGamePlay_Button::Point_Shop_Display_Button()
{

}

void CGamePlay_Button::Spell_Shop_Display_Button()
{

}

void CGamePlay_Button::Upgrade_Weapon_Display_Button()
{

}

HRESULT CGamePlay_Button::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_Button_INFO.strTexture_Default_Tag,
		TEXT("Com_Texture_Menu_1"), reinterpret_cast<CComponent**>(&m_GamePlay_Button_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer_Menu"), reinterpret_cast<CComponent**>(&m_GamePlayer_Button_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_Menu"), reinterpret_cast<CComponent**>(&m_GamePlayer_Button_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CGamePlay_Button* CGamePlay_Button::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGamePlay_Button* pInstance = new CGamePlay_Button(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("게임플레이레벨 버튼 UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CGamePlay_Button::Clone(void* pArg)
{
	CGamePlay_Button* pInstace = new CGamePlay_Button(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("게임 플레이레벨 버튼 UI 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CGamePlay_Button::Free()
{
	__super::Free();

	Safe_Release(m_GamePlay_Button_pTextureCom);
	Safe_Release(m_GamePlayer_Button_pVIBufferCom);
	Safe_Release(m_GamePlayer_Button_pTransformCom);
	
}

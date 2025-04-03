#include "Logo_Button.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CLogo_Button::CLogo_Button(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CLogo_Button::CLogo_Button(const CLogo_Button& Prototype)
	: CUI_Base(Prototype),
	m_Button_pTextureCom(Prototype.m_Button_pTextureCom),
	m_Button_pTransformCom(Prototype.m_Button_pTransformCom),
	m_Button_pVIBufferCom(Prototype.m_Button_pVIBufferCom),
	m_Button_INFO{ Prototype.m_Button_INFO }
{
}

HRESULT CLogo_Button::Initialize_Prototype()
{
	

	return S_OK;
}

HRESULT CLogo_Button::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		m_Button_INFO = *reinterpret_cast<UI_Button_Desc*>(pArg);
		Set_Position(m_Button_INFO.Button_Desc.vPos);
		Set_Size(m_Button_INFO.Button_Desc.vSize);
		CUI_Manager::GetInstance()->AddUI(m_Button_INFO.strUIName, this);
	}
	else
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_Button_pTransformCom->Set_Scale(m_Button_INFO.Button_Desc.vSize.x, m_Button_INFO.Button_Desc.vSize.y, 1.f);
	m_Button_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Button_INFO.Button_Desc.vPos.x, m_Button_INFO.Button_Desc.vPos.y, 0.f));
	return S_OK;
}

void CLogo_Button::Priority_Update(_float fTimeDelta)
{
}

void CLogo_Button::Update(_float fTimeDelta)
{
	if (m_Button_INFO.bStartButton_Flag == true)
	{
		if (true == isPick(g_hWnd))
		{
			m_bIsMouseOver = true;
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
				m_bIsMouseClick = true;
		}
		else
			m_bIsMouseOver = false;
	}
	else if (m_Button_INFO.bExitButton_Flag == true)
	{
		if (isPick(g_hWnd))
		{
			m_bIsMouseOver = true;
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
				PostQuitMessage(0);
		}
		else
		{
			m_bIsMouseOver = false;
		}
	}

}


void CLogo_Button::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}

HRESULT CLogo_Button::Render()
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

	if (FAILED(m_Button_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (m_bIsMouseOver)
	{
		if (FAILED(m_Button_pTextureCom_Second->Bind_Resource(0)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_Button_pTextureCom->Bind_Resource(0)))
			return E_FAIL;
	}

	if (FAILED(m_Button_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_Button_pVIBufferCom->Render()))
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	return S_OK;
}

HRESULT CLogo_Button::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_LOGO, m_Button_INFO.strTexture_Default_Tag,
		TEXT("Com_Texture_Menu_1"), reinterpret_cast<CComponent**>(&m_Button_pTextureCom))))
		return E_FAIL;
	 
	if (FAILED(__super::Add_Component(LEVEL_LOGO, m_Button_INFO.strTexture_Select_Tag,
		TEXT("Com_Texture_Menu_2"), reinterpret_cast<CComponent**>(&m_Button_pTextureCom_Second))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_Button_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_Button_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CLogo_Button* CLogo_Button::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CLogo_Button* pInstance = new CLogo_Button(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("버튼 UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CLogo_Button::Clone(void* pArg)
{
	CLogo_Button* pInstace = new CLogo_Button(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("버튼 UI 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CLogo_Button::Free()
{
	__super::Free();

	Safe_Release(m_Button_pTextureCom);
	Safe_Release(m_Button_pTextureCom_Second);
	Safe_Release(m_Button_pTransformCom);
	Safe_Release(m_Button_pVIBufferCom);
}

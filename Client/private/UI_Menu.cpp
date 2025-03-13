#include "UI_Menu.h"
#include "GameInstance.h"
#include "CUI_Manager.h"

CUI_Menu::CUI_Menu(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CUI_Menu::CUI_Menu(const CUI_Menu& Prototype)
	: CUI_Base(Prototype),
	m_Menu_pTextureCom(Prototype.m_Menu_pTextureCom),
	m_Menu_pTransformCom(Prototype.m_Menu_pTransformCom),
	m_Menu_pVIBufferCom(Prototype.m_Menu_pVIBufferCom),
	m_Menu_INFO{ Prototype.m_Menu_INFO }
{
}

HRESULT CUI_Menu::Initialize_Prototype()
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Menu::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))

		return E_FAIL;

	if (pArg != nullptr)
	{
		m_Menu_INFO = *reinterpret_cast<UI_Child_Desc*>(pArg);
		Set_Position(m_Menu_INFO.vPos);
		Set_Size(m_Menu_INFO.vSize);
		CUI_Manager::GetInstance()->AddUI(L"UI_Menu", this);
	}
	else
		return E_FAIL;


	m_Menu_pTransformCom->Set_Scale(m_Menu_INFO.vSize.x, m_Menu_INFO.vSize.y, 1.f);
	m_Menu_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Menu_INFO.vPos.x, m_Menu_INFO.vPos.y, 0.f));
	return S_OK;
}

void CUI_Menu::Priority_Update(_float fTimeDelta)
{
}

void CUI_Menu::Update(_float fTimeDelta)
{
	if (GetKeyState('F') & 0x8000)
	{
		if (!m_bKeyPressed) 
		{
			m_bIsVisible = !m_bIsVisible;
			m_bKeyPressed = true;
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


	if (FAILED(m_Menu_pTransformCom->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_Menu_pTextureCom->Bind_Resource(0)))
		return E_FAIL;
	if (FAILED(m_Menu_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_Menu_pVIBufferCom->Render()))
		return E_FAIL;


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	return S_OK;
}

HRESULT CUI_Menu::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Menu_Panel"),
		TEXT("Com_Texture_Menu"), reinterpret_cast<CComponent**>(&m_Menu_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Menu_Panel"),
		TEXT("Com_VIBuffer_Menu"), reinterpret_cast<CComponent**>(&m_Menu_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Transform_Menu_Panel"),
		TEXT("Com_Transform_Menu"), reinterpret_cast<CComponent**>(&m_Menu_pTransformCom), &tDesc)))
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

	Safe_Release(m_Menu_pTextureCom);
	Safe_Release(m_Menu_pTransformCom);
	Safe_Release(m_Menu_pVIBufferCom);
}

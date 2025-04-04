#include "Inven_UI.h"
#include "GameInstance.h"
#include "Item_Manager.h" 
#include "UI_Manager.h"

CInven_UI::CInven_UI(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CUI_Base{ pGraphic_Device }
{
}

CInven_UI::CInven_UI(const CInven_UI& Prototype)
	:CUI_Base(Prototype)
{
}

HRESULT CInven_UI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInven_UI::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_Image_INFO.vSize = { 918.f,207.f};
	m_Image_INFO.vPos = { 0.f,160.f };
	m_pTransformCom->Set_Scale(m_Image_INFO.vSize.x, m_Image_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Image_INFO.vPos.x, m_Image_INFO.vPos.y, 0.f));

	CUI_Manager::GetInstance()->AddUI(L"Inven_UI", this);

	m_pWeaponIcon.resize(8);
	return S_OK;
}

void CInven_UI::Priority_Update(_float fTimeDelta)
{
	  static _bool m_bOffItem = { false };

	if (!m_bOffItem)
	{
		for (auto& it : m_pWeaponIcon)
		{
			if (nullptr == it)
				continue;
			it->SetActive(false);
		}

		m_bOffItem = true;
	}
}

void CInven_UI::Update(_float fTimeDelta)
{

	

}

void CInven_UI::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}

HRESULT CInven_UI::Render()
{
	
	if (!m_bRender)
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


	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_Resource(0)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	return S_OK;
}

HRESULT CInven_UI::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Inven_UI"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

CInven_UI* CInven_UI::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CInven_UI* pInstance = new CInven_UI(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("이미지 원본 생성 실패 ");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CInven_UI::Clone(void* pArg)
{
	CInven_UI* pInstace = new CInven_UI(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("이미지 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CInven_UI::Free()
{
	__super::Free();

}

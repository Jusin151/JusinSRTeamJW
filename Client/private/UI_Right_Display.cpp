﻿#include "UI_Right_Display.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CUI_Right_Display::CUI_Right_Display(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CUI_Right_Display::CUI_Right_Display(const CUI_Right_Display& Prototype)
	: CUI_Base(Prototype),
	m_Ammo_pTextureCom(Prototype.m_Ammo_pTextureCom),
	m_Ammo_pTransformCom(Prototype.m_Ammo_pTransformCom),
	m_Ammo_pVIBufferCom(Prototype.m_Ammo_pVIBufferCom),
	m_Ammo_INFO{ Prototype.m_Ammo_INFO }
{
}

HRESULT CUI_Right_Display::Initialize_Prototype()
{
	

	return S_OK;
}

HRESULT CUI_Right_Display::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))

		return E_FAIL;

	if (pArg != nullptr)
	{
		m_Ammo_INFO = *reinterpret_cast<UI_Child_Desc*>(pArg);
		m_Ammo_INFO.vPos += CUI_Manager::GetInstance()->GetParent_Pos();
		Set_Position(m_Ammo_INFO.vPos);
		Set_Size(m_Ammo_INFO.vSize);
		CUI_Manager::GetInstance()->AddUI(L"Right_Panel", this);
	}
	else
		return E_FAIL;


	m_Ammo_pTransformCom->Set_Scale(m_Ammo_INFO.vSize.x, m_Ammo_INFO.vSize.y, 1.f);
	m_Ammo_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Ammo_INFO.vPos.x, m_Ammo_INFO.vPos.y, 0.f));
	return S_OK;
}

void CUI_Right_Display::Priority_Update(_float fTimeDelta)
{
}

void CUI_Right_Display::Update(_float fTimeDelta)
{
}

void CUI_Right_Display::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI_BACKGROUND, this)))
		return;
}

HRESULT CUI_Right_Display::Render()
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


	if (FAILED(m_Ammo_pTransformCom->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_Ammo_pTextureCom->Bind_Resource(0)))
		return E_FAIL;
	if (FAILED(m_Ammo_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_Ammo_pVIBufferCom->Render()))
		return E_FAIL;


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	return S_OK;
}

HRESULT CUI_Right_Display::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Right_Panel"),
		TEXT("Com_Texture_Ammo"), reinterpret_cast<CComponent**>(&m_Ammo_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer_Ammo"), reinterpret_cast<CComponent**>(&m_Ammo_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_Ammo"), reinterpret_cast<CComponent**>(&m_Ammo_pTransformCom), &tDesc)))
  		return E_FAIL;

	return S_OK;
}

CUI_Right_Display* CUI_Right_Display::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Right_Display* pInstance = new CUI_Right_Display(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("탄약 UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CUI_Right_Display::Clone(void* pArg)
{
	CUI_Right_Display* pInstace = new CUI_Right_Display(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("탄약 UI 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CUI_Right_Display::Free()
{
	__super::Free();

	Safe_Release(m_Ammo_pTextureCom);
	Safe_Release(m_Ammo_pTransformCom);
	Safe_Release(m_Ammo_pVIBufferCom);
}

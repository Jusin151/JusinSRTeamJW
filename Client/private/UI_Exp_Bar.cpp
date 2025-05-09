﻿#include "UI_HP_Bar.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CUI_Exp_Bar::CUI_Exp_Bar(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CUI_Exp_Bar::CUI_Exp_Bar(const CUI_Exp_Bar& Prototype)
	: CUI_Base(Prototype)
{
}

HRESULT CUI_Exp_Bar::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CUI_Exp_Bar::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		m_Exp_Bar_INFO = *reinterpret_cast<UI_Child_Desc*>(pArg);
		m_Exp_Bar_INFO.vPos += CUI_Manager::GetInstance()->GetMidPanel_Pos();
		Set_Position(m_Exp_Bar_INFO.vPos);
		Set_Size(m_Exp_Bar_INFO.vSize);
		CUI_Manager::GetInstance()->AddUI(L"Exp_Bar", this);

	}
	else
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_pTransformCom->Set_Scale(m_Exp_Bar_INFO.vSize.x, m_Exp_Bar_INFO.vSize.y, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Exp_Bar_INFO.vPos.x, m_Exp_Bar_INFO.vPos.y, 0.f));

	return S_OK;
}

void CUI_Exp_Bar::Priority_Update(_float fTimeDelta)
{
}

void CUI_Exp_Bar::Update(_float fTimeDelta)
{
	if (m_fExpTextDuration > 0.f)
		m_fExpTextDuration -= fTimeDelta;

	//Update_Exp_Bar();
}

void CUI_Exp_Bar::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;

	//Update_HP_Bar();
}
void CUI_Exp_Bar::Update_Exp_Bar()
{
	_float fExp_Ratio = (_float)m_iExp / 100.f;
	fExp_Ratio = max(0.f, min(1.f, fExp_Ratio));  // [0,1] 클램핑

	VTXNORTEX* pVertices = nullptr;
	m_pVIBufferCom->Get_VertexBuffer()->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);


	pVertices[0].vTexcoord.x = 0.f;               // 좌상단
	pVertices[3].vTexcoord.x = 0.f;               // 좌하단
	pVertices[1].vTexcoord.x = fExp_Ratio;        // 우상단
	pVertices[2].vTexcoord.x = fExp_Ratio;        // 우하단


	pVertices[0].vPosition.x = -0.5f;                        // 좌상단
	pVertices[3].vPosition.x = -0.5f;                        // 좌하단 
	pVertices[1].vPosition.x = -0.5f + fExp_Ratio;           // 우상단 
	pVertices[2].vPosition.x = -0.5f + fExp_Ratio;           // 우하단 

	m_pVIBufferCom->Get_VertexBuffer()->Unlock();
}




HRESULT CUI_Exp_Bar::Render()
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

	if (m_fExpTextDuration > 0.f)
	{
		_float2 vTextPos = _float2(-600.f, 150.f); 
		_float2 vTextSize = _float2(8.f, 20.f);    
		_float3 vTextColor = _float3(1.f, 1.f, 0.f); 

		m_pGameInstance->Render_Font_Size(L"MainFont", m_strExpText, vTextPos, vTextSize, vTextColor);
	}


	return S_OK;
}

HRESULT CUI_Exp_Bar::Ready_Components()
{

   	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ExpBar_UI"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
       		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_EXP"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}


CUI_Exp_Bar* CUI_Exp_Bar::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Exp_Bar* pInstance = new CUI_Exp_Bar(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("ExpBar UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Exp_Bar::Clone(void* pArg)
{
	CUI_Exp_Bar* pInstace = new CUI_Exp_Bar(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("ExpBar UI 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CUI_Exp_Bar::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}

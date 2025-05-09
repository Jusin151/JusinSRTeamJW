﻿#include "UI_Bullet_Bar.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CUI_Bullet_Bar::CUI_Bullet_Bar(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CUI_Bullet_Bar::CUI_Bullet_Bar(const CUI_Bullet_Bar& Prototype)
	: CUI_Base(Prototype),
	m_Bullet_Bar_INFO{ Prototype.m_Bullet_Bar_INFO }
{
}

HRESULT CUI_Bullet_Bar::Initialize_Prototype()
{
	

	return S_OK;
}

HRESULT CUI_Bullet_Bar::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))

		return E_FAIL;

	if (pArg != nullptr)
	{
		m_Bullet_Bar_INFO = *reinterpret_cast<UI_Child_Desc*>(pArg);
		m_Bullet_Bar_INFO.vPos += CUI_Manager::GetInstance()->GetRightPanel_Pos();
		Set_Position(m_Bullet_Bar_INFO.vPos);
		Set_Size(m_Bullet_Bar_INFO.vSize);
		CUI_Manager::GetInstance()->AddUI(L"Bullet_Bar", this);
	}
	else
		return E_FAIL;

	m_pTransformCom->Set_Scale(m_Bullet_Bar_INFO.vSize.x, m_Bullet_Bar_INFO.vSize.y, 1.f);


	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Bullet_Bar_INFO.vPos.x, m_Bullet_Bar_INFO.vPos.y, 0.f));
	return S_OK;
}

void CUI_Bullet_Bar::Priority_Update(_float fTimeDelta)
{
}

void CUI_Bullet_Bar::Update(_float fTimeDelta)
{
	int a = 10;
	//////////////////

}

void CUI_Bullet_Bar::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;

}
void CUI_Bullet_Bar::Update_Bullet_Bar()
{
	_float fBullet_Ratio = (float)m_iBullet / (float)m_iMxBullet;
	if (fBullet_Ratio < 0.f)
		fBullet_Ratio = 0.f;
	if (fBullet_Ratio > 1.f)
		fBullet_Ratio = 1.f;

	VTXNORTEX* pVertices = nullptr;
	m_pVIBufferCom->Get_VertexBuffer()->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);

	// 전체 이미지가 보이는 상태(fBullet_Ratio==1)면 좌측 texcoord.x = 0,
	// fBullet_Ratio가 줄어들면 좌측 부분만 보이도록 (예: 0.5이면 texcoord.x = 0.5)
	pVertices[0].vTexcoord.x = 1.0f - fBullet_Ratio; // 좌측 상단
	pVertices[3].vTexcoord.x = 1.0f - fBullet_Ratio; // 좌측 하단

	// 전체 상태(fBullet_Ratio == 1): 좌측 x = -0.5
	// fBullet_Ratio가 줄어들면 좌측 x = -0.5 + (1 - fBullet_Ratio)
	pVertices[0].vPosition.x = -0.5f + (1.0f - fBullet_Ratio);
	pVertices[3].vPosition.x = -0.5f + (1.0f - fBullet_Ratio);

	m_pVIBufferCom->Get_VertexBuffer()->Unlock();
}



HRESULT CUI_Bullet_Bar::Render()
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

	if(m_iMxBullet!=0)
	m_pGameInstance->Render_Font_Size(L"MainFont", to_wstring(m_iBullet), _float2(583.f, 320.f), _float2(15.f, 25.f), _float3(1.f, 1.f, 1.f));




	return S_OK;
}

HRESULT CUI_Bullet_Bar::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bullet_Bar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Bullet"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_Bullet_Bar* CUI_Bullet_Bar::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Bullet_Bar* pInstance = new CUI_Bullet_Bar(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("총알바 UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CUI_Bullet_Bar::Clone(void* pArg)
{
	CUI_Bullet_Bar* pInstace = new CUI_Bullet_Bar(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("총알바 UI 클론 생성 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CUI_Bullet_Bar::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}

#include "UI_HP_Bar.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CUI_HP_Bar::CUI_HP_Bar(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CUI_HP_Bar::CUI_HP_Bar(const CUI_HP_Bar& Prototype)
	: CUI_Base(Prototype),
	m_HP_Bar_INFO{ Prototype.m_HP_Bar_INFO }
{
}

HRESULT CUI_HP_Bar::Initialize_Prototype()
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_HP_Bar::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))

		return E_FAIL;

	if (pArg != nullptr)
	{
		m_HP_Bar_INFO = *reinterpret_cast<UI_Child_Desc*>(pArg);
		m_HP_Bar_INFO.vPos += CUI_Manager::GetInstance()->GetLeftPanel_Pos();
		Set_Position(m_HP_Bar_INFO.vPos);
		Set_Size(m_HP_Bar_INFO.vSize);
		CUI_Manager::GetInstance()->AddUI(L"Hp_Bar", this);

	}
	else
		return E_FAIL;



	m_pTransformCom->Set_Scale(m_HP_Bar_INFO.vSize.x, m_HP_Bar_INFO.vSize.y, 1.f);


	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_HP_Bar_INFO.vPos.x, m_HP_Bar_INFO.vPos.y, 0.f));

	return S_OK;
}

void CUI_HP_Bar::Priority_Update(_float fTimeDelta)
{
}

void CUI_HP_Bar::Update(_float fTimeDelta)
{

}

void CUI_HP_Bar::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;

	Update_HP_Bar();
}
void CUI_HP_Bar::Update_HP_Bar()
{
	_float fHP_Ratio = m_iHealth / 100.f;

	if (fHP_Ratio < 0.f)
		fHP_Ratio = 0.f;
	if (fHP_Ratio > 1.f)
		fHP_Ratio = 1.f;


	VTXNORTEX* pVertices = nullptr;
	m_pVIBufferCom->Get_VertexBuffer()->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);

	//  (오른쪽부터 점점 안 보이게)
	pVertices[1].vTexcoord.x = fHP_Ratio; // 우측 상단
	pVertices[2].vTexcoord.x = fHP_Ratio; // 우측 하단

	//  정점 위치  (오른쪽부터 점점 줄어들게) 
	_float fNewWidth = m_HP_Bar_INFO.vSize.x * fHP_Ratio;
	pVertices[1].vPosition.x = -0.5f + fNewWidth / m_HP_Bar_INFO.vSize.x;
	pVertices[2].vPosition.x = -0.5f + fNewWidth / m_HP_Bar_INFO.vSize.x;

	m_pVIBufferCom->Get_VertexBuffer()->Unlock();
}


HRESULT CUI_HP_Bar::Render()
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
	if (FAILED(m_pTextureCom->Bind_Resource(iCurrent_Image)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);


	m_pGameInstance->Render_Font_Size(L"MainFont", to_wstring(m_iHealth), _float2(-522.f, 325.0f),_float2(10.f,20.f) ,_float3(1.f, 1.f, 1.f));

	 return S_OK;
}

HRESULT CUI_HP_Bar::Ready_Components()
{

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Hp_Bar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_HP"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}


CUI_HP_Bar* CUI_HP_Bar::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_HP_Bar* pInstance = new CUI_HP_Bar(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("체력바 UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_HP_Bar::Clone(void* pArg)
{
	CUI_HP_Bar* pInstace = new CUI_HP_Bar(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("체력바 UI 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CUI_HP_Bar::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}

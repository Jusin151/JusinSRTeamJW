#include "UI_Mid_Display.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CUI_Mid_Display::CUI_Mid_Display(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CUI_Mid_Display::CUI_Mid_Display(const CUI_Mid_Display& Prototype)
	: CUI_Base(Prototype),
	m_EXP_INFO{ Prototype.m_EXP_INFO }
{
}

HRESULT CUI_Mid_Display::Initialize_Prototype()
{
	


	return S_OK;
}

HRESULT CUI_Mid_Display::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))

		return E_FAIL;

	if (pArg != nullptr)
	{
		m_EXP_INFO = *reinterpret_cast<UI_Child_Desc*>(pArg);
		

			m_EXP_INFO.vPos += CUI_Manager::GetInstance()->GetParent_Pos();
			Set_Position(m_EXP_INFO.vPos);
			Set_Size(m_EXP_INFO.vSize);
			CUI_Manager::GetInstance()->AddUI(L"Mid_Panel", this);
			iCurrent_Image = 0;
	}
	else
		return E_FAIL;




	m_pTransformCom->Set_Scale(m_EXP_INFO.vSize.x, m_EXP_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_EXP_INFO.vPos.x, m_EXP_INFO.vPos.y, 0.f));
	return S_OK;
}

void CUI_Mid_Display::Priority_Update(_float fTimeDelta)
{
}

void CUI_Mid_Display::Update(_float fTimeDelta)
{
	
}

void CUI_Mid_Display::Update_Exp_Bar()
{
	
}

void CUI_Mid_Display::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}

HRESULT CUI_Mid_Display::Render()
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

	return S_OK;
}

HRESULT CUI_Mid_Display::Ready_Components()
{

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Mid_Panel"),
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

CUI_Mid_Display* CUI_Mid_Display::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Mid_Display* pInstance = new CUI_Mid_Display(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("경험치바 UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CUI_Mid_Display::Clone(void* pArg)
{
	CUI_Mid_Display* pInstace = new CUI_Mid_Display(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("경험치바 UI 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CUI_Mid_Display::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}

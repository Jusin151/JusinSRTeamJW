#include "UI_HP_Bar.h"
#include "GameInstance.h"
#include "CUI_Manager.h"

CUI_HP_Bar::CUI_HP_Bar(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CUI_HP_Bar::CUI_HP_Bar(const CUI_HP_Bar& Prototype)
	: CUI_Base(Prototype),
	m_HP_Bar_pTextureCom(Prototype.m_HP_Bar_pTextureCom),
	m_HP_Bar_pTransformCom(Prototype.m_HP_Bar_pTransformCom),
	m_HP_Bar_pVIBufferCom(Prototype.m_HP_Bar_pVIBufferCom),
	m_HP_Bar_INFO{ Prototype.m_HP_Bar_INFO }
{
}

HRESULT CUI_HP_Bar::Initialize_Prototype()
{
	if (FAILED(Ready_Components()))
	{
		return E_FAIL;
	}

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

	m_HP_Bar_pTransformCom->Set_Scale(m_HP_Bar_INFO.vSize.x, m_HP_Bar_INFO.vSize.y, 1.f);


	m_HP_Bar_pTransformCom->Set_State(CTransform::STATE_POSITION,
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


	if (FAILED(m_HP_Bar_pTransformCom->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_HP_Bar_pTextureCom->Bind_Resource(0)))
		return E_FAIL;
	if (FAILED(m_HP_Bar_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_HP_Bar_pVIBufferCom->Render()))
		return E_FAIL;


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	return S_OK;
}

HRESULT CUI_HP_Bar::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Hp_Bar"),
		TEXT("Com_Texture_HP"), reinterpret_cast<CComponent**>(&m_HP_Bar_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Hp_Bar"),
		TEXT("Com_VIBuffer_HP"), reinterpret_cast<CComponent**>(&m_HP_Bar_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Transform_Hp_Bar"),
		TEXT("Com_Transform_HP"), reinterpret_cast<CComponent**>(&m_HP_Bar_pTransformCom), &tDesc)))
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

	Safe_Release(m_HP_Bar_pTextureCom);
	Safe_Release(m_HP_Bar_pTransformCom);
	Safe_Release(m_HP_Bar_pVIBufferCom);
}

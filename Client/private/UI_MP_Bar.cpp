#include "UI_MP_Bar.h"
#include "GameInstance.h"
#include "CUI_Manager.h"

CUI_MP_Bar::CUI_MP_Bar(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CUI_MP_Bar::CUI_MP_Bar(const CUI_MP_Bar& Prototype)
	: CUI_Base(Prototype),
	m_MP_pTextureCom(Prototype.m_MP_pTextureCom),
	m_MP_pTransformCom(Prototype.m_MP_pTransformCom),
	m_MP_pVIBufferCom(Prototype.m_MP_pVIBufferCom),
	m_MP_INFO{ Prototype.m_MP_INFO }
{
}

HRESULT CUI_MP_Bar::Initialize_Prototype()
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_MP_Bar::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))

		return E_FAIL;

	if (pArg != nullptr)
	{
		m_MP_INFO = *reinterpret_cast<UI_Child_Desc*>(pArg);
		m_MP_INFO.vPos += CUI_Manager::GetInstance()->GetLeftPanel_Pos();
		Set_Position(m_MP_INFO.vPos);
		Set_Size(m_MP_INFO.vSize);
		CUI_Manager::GetInstance()->AddUI(L"MP_Bar", this);
	}
	else
		return E_FAIL;


	m_fMp = 50.f;
	m_MP_pTransformCom->Set_Scale(m_MP_INFO.vSize.x, m_MP_INFO.vSize.y, 1.f);
	m_MP_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_MP_INFO.vPos.x, m_MP_INFO.vPos.y, 0.f));
	return S_OK;
}

void CUI_MP_Bar::Priority_Update(_float fTimeDelta)
{
}

void CUI_MP_Bar::Update(_float fTimeDelta)
{
	if (GetAsyncKeyState('9') & 0x8000)
	{
		m_fMp -= 1.f;
		if (m_fMp < 0.f)
			m_fMp = 0.f; // ÃÖ¼Ò ¸¶³ª Á¦ÇÑ


	   Update_Mp_Bar();
	}
}

void CUI_MP_Bar::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}
void CUI_MP_Bar::Update_Mp_Bar()
{
	_float fHP_Ratio = m_fMp / 50.f;

	if (fHP_Ratio < 0.f)
		fHP_Ratio = 0.f;
	if (fHP_Ratio > 1.f)
		fHP_Ratio = 1.f;


	VTXPOSTEX* pVertices = nullptr;
	m_MP_pVIBufferCom->Get_VertexBuffer()->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);

	//  (¿À¸¥ÂÊºÎÅÍ Á¡Á¡ ¾È º¸ÀÌ°Ô)
	pVertices[1].vTexcoord.x = fHP_Ratio; // ¿ìÃø »ó´Ü
	pVertices[2].vTexcoord.x = fHP_Ratio; // ¿ìÃø ÇÏ´Ü

	//  Á¤Á¡ À§Ä¡  (¿À¸¥ÂÊºÎÅÍ Á¡Á¡ ÁÙ¾îµé°Ô) 
	float fNewWidth = m_MP_INFO.vSize.x * fHP_Ratio;
	pVertices[1].vPosition.x = -0.5f + fNewWidth / m_MP_INFO.vSize.x;
	pVertices[2].vPosition.x = -0.5f + fNewWidth / m_MP_INFO.vSize.x;

	m_MP_pVIBufferCom->Get_VertexBuffer()->Unlock();
}


HRESULT CUI_MP_Bar::Render()
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


	if (FAILED(m_MP_pTransformCom->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_MP_pTextureCom->Bind_Resource(0)))
		return E_FAIL;
	if (FAILED(m_MP_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_MP_pVIBufferCom->Render()))
		return E_FAIL;


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	return S_OK;
}

HRESULT CUI_MP_Bar::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MP_Bar"),
		TEXT("Com_Texture_Mana"), reinterpret_cast<CComponent**>(&m_MP_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_MP_Bar"),
		TEXT("Com_VIBuffer_Mana"), reinterpret_cast<CComponent**>(&m_MP_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Transform_MP_Bar"),
		TEXT("Com_Transform_Mana"), reinterpret_cast<CComponent**>(&m_MP_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_MP_Bar* CUI_MP_Bar::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_MP_Bar* pInstance = new CUI_MP_Bar(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("íƒ„ì•½ UI ì›ë³¸ ìƒì„± ì‹¤íŒ¨ ");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CUI_MP_Bar::Clone(void* pArg)
{
	CUI_MP_Bar* pInstace = new CUI_MP_Bar(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("íƒ„ì•½ UI ë³µì œ ì‹¤íŒ¨");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CUI_MP_Bar::Free()
{
	__super::Free();

	Safe_Release(m_MP_pTextureCom);
	Safe_Release(m_MP_pTransformCom);
	Safe_Release(m_MP_pVIBufferCom);
}

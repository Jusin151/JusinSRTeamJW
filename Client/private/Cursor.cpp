#include "Cursor.h"
#include "UI_Manager.h"
#include "GameInstance.h"
CCursor::CCursor(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CUI_Base(pGraphic_Device)
{
}

CCursor::CCursor(const CCursor& rhs)
	: CUI_Base(rhs)
{
}

HRESULT CCursor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCursor::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	CUI_Manager::GetInstance()->AddUI(L"Cursor", this);
	return S_OK;
}

void CCursor::Priority_Update(_float fTimeDelta)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	_float3 vMousePos{ _float(pt.x),_float(pt.y),0.f };

	if (m_pTransformCom)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMousePos);
	}
}

void CCursor::Update(_float fTimeDelta)
{
}

void CCursor::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}

HRESULT CCursor::Render()
{
	if (FAILED(m_pTextureCom->Bind_Resource(m_bMousePressed)))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	SetUp_RenderState();

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	Release_RenderState();

	return S_OK;
}

HRESULT CCursor::SetUp_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, false);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 100);
	return S_OK;
}

HRESULT CCursor::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	return S_OK;
}

HRESULT CCursor::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cursor"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

CCursor* CCursor::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCursor* pInstance = new CCursor(pGraphic_Device);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to create CCursor instance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCursor::Clone(void* pArg)
{
	CCursor* pClone = new CCursor(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("Failed to clone CCursor instance");
		Safe_Release(pClone);
	}
	return pClone;
}

void CCursor::Free()
{
	__super::Free();
}

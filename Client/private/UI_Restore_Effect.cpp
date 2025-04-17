#include "UI_Restore_Effect.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CUI_Restore_Effect::CUI_Restore_Effect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base{ pGraphic_Device }
{
}

CUI_Restore_Effect::CUI_Restore_Effect(const CUI_Restore_Effect& Prototype)
	: CUI_Base{ Prototype }
{
}

HRESULT CUI_Restore_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Restore_Effect::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	m_pTransformCom->Set_Scale(1280.f, 720.f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _float3(0.f, 0.f, 0.f));
	CUI_Manager::GetInstance()->AddUI(L"Restore_Effect", this);
	return S_OK;
}

HRESULT CUI_Restore_Effect::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Restore"),
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

void CUI_Restore_Effect::Priority_Update(_float fTimeDelta)
{

}

void CUI_Restore_Effect::Update(_float fTimeDelta)
{
	// 활성화 상태가 아니면 업데이트하지 않음
	if (!m_bStart)
	{
		return;
	}

	m_fElpasedTime += fTimeDelta;

	if (m_fElpasedTime >= m_fAnimationTime)
	{
		m_fAlpha = 0.0f;
		m_bStart = false;
	}
	else
	{
		float fProgress = m_fElpasedTime / m_fAnimationTime;
		m_fAlpha = Lerp(0.4f, 0.0f, fProgress);
	}
}

void CUI_Restore_Effect::Late_Update(_float fTimeDelta)
{
	if (m_bStart)
		m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI_BACKGROUND, this);
}

HRESULT CUI_Restore_Effect::Pre_Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	DWORD alphaFactorDWORD = (DWORD)(m_fAlpha * 255.0f);
	DWORD textureFactorColor = D3DCOLOR_ARGB(alphaFactorDWORD, 255, 255, 255);
	m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, textureFactorColor);

	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphic_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	return S_OK;
}

HRESULT CUI_Restore_Effect::Render()
{
	Pre_Render();
	D3DXMATRIX matOldView, matOldProj;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matOldProj);

	D3DXMATRIX matView;
	D3DXMatrixIdentity(&matView);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIX matProj;
	D3DXMatrixOrthoLH(&matProj, g_iWinSizeX, g_iWinSizeY, 0.f, 1.f);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matProj);

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_Resource(0)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	Post_Render();
	return S_OK;
}

HRESULT CUI_Restore_Effect::Post_Render()
{
	DWORD textureFactorColor = D3DCOLOR_ARGB(255, 255, 255, 255);
	m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, textureFactorColor);
	return S_OK;
}

void CUI_Restore_Effect::StartEffect()
{
	m_bStart = true;
	m_fElpasedTime = 0.0f;
	m_fAlpha = 0.4f;        // 시작 알파
	m_fAnimationTime = 1.0f;
}

CUI_Restore_Effect* CUI_Restore_Effect::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Restore_Effect* pInstance = new CUI_Restore_Effect(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUI_Restore_Effect 원본 생성 실패 ");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Restore_Effect::Clone(void* pArg)
{
	CUI_Restore_Effect* pInstace = new CUI_Restore_Effect(*this);
	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("CUI_Restore_Effect 복제 실패");
		Safe_Release(pInstace);
	}
	return pInstace;
}

void CUI_Restore_Effect::Free()
{
	__super::Free();
}

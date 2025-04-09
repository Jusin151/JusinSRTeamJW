#include "UI_Hit_Blood.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CUI_Hit_Blood::CUI_Hit_Blood(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base { pGraphic_Device }
{
}

CUI_Hit_Blood::CUI_Hit_Blood(const CUI_Hit_Blood& Prototype)
	: CUI_Base { Prototype }
{
}

HRESULT CUI_Hit_Blood::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Hit_Blood::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CUI_Hit_Blood::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Hit"),
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

void CUI_Hit_Blood::Priority_Update(_float fTimeDelta)
{

}

void CUI_Hit_Blood::Update(_float fTimeDelta)
{
	m_fElpasedTime += fTimeDelta;
	if (m_fElpasedTime >= m_fAnimationTime)
	{
		//m_fAlpha
	}
	m_bIsActive = false;
}

void CUI_Hit_Blood::Late_Update(_float fTimeDelta)
{
	if(m_bIsActive)
		m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI_BACKGROUND, this);
}

HRESULT CUI_Hit_Blood::Pre_Render()
{
	return S_OK;
}

HRESULT CUI_Hit_Blood::Render()
{
	return S_OK;
}

HRESULT CUI_Hit_Blood::Post_Render()
{
	return S_OK;
}

CUI_Hit_Blood* CUI_Hit_Blood::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Hit_Blood* pInstance = new CUI_Hit_Blood(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUI_Hit_Blood 원본 생성 실패 ");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Hit_Blood::Clone(void* pArg)
{
	CUI_Hit_Blood* pInstace = new CUI_Hit_Blood(*this);
	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("CUI_Hit_Blood 복제 실패");
		Safe_Release(pInstace);
	}
	return pInstace;
}

void CUI_Hit_Blood::Free()
{
}

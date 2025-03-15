#include "UI_Episode.h"
#include "GameInstance.h"
#include "CUI_Manager.h"

CUI_Episode::CUI_Episode(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CUI_Episode::CUI_Episode(const CUI_Episode& Prototype)
	: CUI_Base(Prototype),
	m_EpisodeUI_pTextureCom(Prototype.m_EpisodeUI_pTextureCom),
	m_EpisodeUI_pTransformCom(Prototype.m_EpisodeUI_pTransformCom),
	m_EpisodeUI_pVIBufferCom(Prototype.m_EpisodeUI_pVIBufferCom),
	m_EpisodeUI_INFO{ Prototype.m_EpisodeUI_INFO }
{
}

HRESULT CUI_Episode::Initialize_Prototype()
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Episode::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))

		return E_FAIL;

	if (pArg != nullptr)
	{
		m_EpisodeUI_INFO = *reinterpret_cast<UI_Child_Desc*>(pArg);
		Set_Position(m_EpisodeUI_INFO.vPos);
		Set_Size(m_EpisodeUI_INFO.vSize);
		CUI_Manager::GetInstance()->AddUI(L"UI_Episode", this);
	}
	else
		return E_FAIL;


	m_EpisodeUI_pTransformCom->Set_Scale(m_EpisodeUI_INFO.vSize.x, m_EpisodeUI_INFO.vSize.y, 1.f);
	m_EpisodeUI_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_EpisodeUI_INFO.vPos.x, m_EpisodeUI_INFO.vPos.y, 0.f));
	return S_OK;
}

void CUI_Episode::Priority_Update(_float fTimeDelta)
{
}

void CUI_Episode::Update(_float fTimeDelta)
{
	if (GetKeyState('3') & 0x8000)
	{
		if (!m_bKeyPressed)
		{
			m_bIsVisible = !m_bIsVisible;
			m_bKeyPressed = true;
			
		}
		//CUI_Manager::GetInstance()->Set_Episode_Display(true);
	}
	else
	{
		m_bKeyPressed = false;
		//CUI_Manager::GetInstance()->Set_Episode_Display(false);
	}
}


void CUI_Episode::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}

HRESULT CUI_Episode::Render()
{
	if (!m_bIsVisible) // 이미지가 표시되지 않으면 렌더링 ㄴ
		return S_OK;

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


	if (FAILED(m_EpisodeUI_pTransformCom->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_EpisodeUI_pTextureCom->Bind_Resource(0)))
		return E_FAIL;
	if (FAILED(m_EpisodeUI_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_EpisodeUI_pVIBufferCom->Render()))
		return E_FAIL;


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	return S_OK;
}

HRESULT CUI_Episode::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Episode_UI"),
		TEXT("Com_Texture_Menu"), reinterpret_cast<CComponent**>(&m_EpisodeUI_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer_Menu"), reinterpret_cast<CComponent**>(&m_EpisodeUI_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_Menu"), reinterpret_cast<CComponent**>(&m_EpisodeUI_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_Episode* CUI_Episode::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Episode* pInstance = new CUI_Episode(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("메뉴 UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CUI_Episode::Clone(void* pArg)
{
	CUI_Episode* pInstace = new CUI_Episode(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("메뉴 UI 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CUI_Episode::Free()
{
	__super::Free();

	Safe_Release(m_EpisodeUI_pTextureCom);
	Safe_Release(m_EpisodeUI_pTransformCom);
	Safe_Release(m_EpisodeUI_pVIBufferCom);
}

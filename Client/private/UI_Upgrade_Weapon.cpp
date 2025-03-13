#include "UI_Upgrade_Weapon.h"
#include "GameInstance.h"
#include "CUI_Manager.h"

CUI_Upgrade_Weapon::CUI_Upgrade_Weapon(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI_Base(pGraphic_Device)
{
}

CUI_Upgrade_Weapon::CUI_Upgrade_Weapon(const CUI_Upgrade_Weapon& Prototype)
	: CUI_Base(Prototype),
	m_Upgrade_UI_pTextureCom(Prototype.m_Upgrade_UI_pTextureCom),
	m_Upgrade_UI_pTransformCom(Prototype.m_Upgrade_UI_pTransformCom),
	m_Upgrade_UI_pVIBufferCom(Prototype.m_Upgrade_UI_pVIBufferCom),
	m_Upgrade_UI_INFO{ Prototype.m_Upgrade_UI_INFO }
{
}

HRESULT CUI_Upgrade_Weapon::Initialize_Prototype()
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Upgrade_Weapon::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))

		return E_FAIL;

	if (pArg != nullptr)
	{
		m_Upgrade_UI_INFO = *reinterpret_cast<UI_Child_Desc*>(pArg);
		Set_Position(m_Upgrade_UI_INFO.vPos);
		Set_Size(m_Upgrade_UI_INFO.vSize);
		CUI_Manager::GetInstance()->AddUI(L"UI_Upgrade_Weapon", this);
	}
	else
		return E_FAIL;


	m_Upgrade_UI_pTransformCom->Set_Scale(m_Upgrade_UI_INFO.vSize.x, m_Upgrade_UI_INFO.vSize.y, 1.f);
	m_Upgrade_UI_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Upgrade_UI_INFO.vPos.x, m_Upgrade_UI_INFO.vPos.y, 0.f));
	return S_OK;
}

void CUI_Upgrade_Weapon::Priority_Update(_float fTimeDelta)
{
}

void CUI_Upgrade_Weapon::Update(_float fTimeDelta)
{
	if (GetKeyState('5') & 0x8000)
	{
		if (!m_bKeyPressed)
		{
			m_bIsVisible = !m_bIsVisible;
			m_bKeyPressed = true;
		}
	}
	else
	{
		m_bKeyPressed = false;
	}
}


void CUI_Upgrade_Weapon::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}

HRESULT CUI_Upgrade_Weapon::Render()
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


	if (FAILED(m_Upgrade_UI_pTransformCom->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_Upgrade_UI_pTextureCom->Bind_Resource(0)))
		return E_FAIL;
	if (FAILED(m_Upgrade_UI_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_Upgrade_UI_pVIBufferCom->Render()))
		return E_FAIL;


	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

	return S_OK;
}

HRESULT CUI_Upgrade_Weapon::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Upgrade_Weapon_UI"),
		TEXT("Com_Texture_Menu"), reinterpret_cast<CComponent**>(&m_Upgrade_UI_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Upgrade_Weapon_UI"),
		TEXT("Com_VIBuffer_Menu"), reinterpret_cast<CComponent**>(&m_Upgrade_UI_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Transform_Upgrade_Weapon_UI"),
		TEXT("Com_Transform_Menu"), reinterpret_cast<CComponent**>(&m_Upgrade_UI_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CUI_Upgrade_Weapon* CUI_Upgrade_Weapon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUI_Upgrade_Weapon* pInstance = new CUI_Upgrade_Weapon(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("메뉴 UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CUI_Upgrade_Weapon::Clone(void* pArg)
{
	CUI_Upgrade_Weapon* pInstace = new CUI_Upgrade_Weapon(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("메뉴 UI 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CUI_Upgrade_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_Upgrade_UI_pTextureCom);
	Safe_Release(m_Upgrade_UI_pTransformCom);
	Safe_Release(m_Upgrade_UI_pVIBufferCom);
}

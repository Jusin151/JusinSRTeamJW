#include "Item_Icon.h"
#include "GameInstance.h"
#include "Item_Manager.h"



CItem_Icon::CItem_Icon(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CItem_Icon::CItem_Icon(const CItem_Icon& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CItem_Icon::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CItem_Icon::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pArg != nullptr)
	{
		m_Icon_INFO = *reinterpret_cast<Icon_DESC*>(pArg);
	}
	m_Icon_INFO.vSize = { 90.f,34.f }; //초기 값

	switch (m_Icon_INFO.Icon_Image)
	{
	case Client::CItem_Icon::Claymore:
		m_Icon_INFO.vPos = { -400.f,200.f };
		break;
	case Client::CItem_Icon::Axe:
		m_Icon_INFO.vPos = { -300.f,200.f };
		break;
	case Client::CItem_Icon::ShotGun:
		m_Icon_INFO.vPos = { -200.f,200.f };
		break;
	case Client::CItem_Icon::Magnum:
		m_Icon_INFO.vPos = { -100.f,200.f };
		break;
	case Client::CItem_Icon::Staff:
		m_Icon_INFO.vPos = { 0.f,200.f };
		break;
	default:
		break;
	}
	m_pTransformCom->Set_Scale(m_Icon_INFO.vSize.x, m_Icon_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Icon_INFO.vPos.x, m_Icon_INFO.vPos.y, 0.f));


	m_ItemManager = CItem_Manager::GetInstance();
	return S_OK;
}
void CItem_Icon::Update(_float fTimeDelta)
{
	static const unordered_map<wstring, Enum_Weapon> weaponMap = 
	{
		{L"Claymore", Claymore},
		{L"Axe", Axe},
		{L"ShotGun", ShotGun},
		{L"Magnum", Magnum},
		{L"Staff", Staff}
	};
	for (const auto& weapon : weaponMap)
	{
		if (m_ItemManager->Get_Current_Weapon_Active(weapon.first))
		{
			if (m_Icon_INFO.Icon_Image == weapon.second)
			{
				m_Icon_INFO.vSize = { 150.f, 75.f };
			}
			else
			{
				m_Icon_INFO.vSize = { 90.f, 34.f }; // 선택 되기 이전의 크기로
			}
			break;
		}
	}

	m_pTransformCom->Set_Scale(m_Icon_INFO.vSize.x, m_Icon_INFO.vSize.y, 1.f);
}

void CItem_Icon::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}


HRESULT CItem_Icon::Render()
{
	if (!m_ItemManager->Get_Inven_Render())
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

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_Resource(m_Icon_INFO.Icon_Image)))
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

HRESULT CItem_Icon::Ready_Components()
{

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Weapon_Icon"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer_Rect"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}


CItem_Icon* CItem_Icon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CItem_Icon* pInstance = new CItem_Icon(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("아이콘 UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem_Icon::Clone(void* pArg)
{
	CItem_Icon* pInstace = new CItem_Icon(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("아이콘 UI 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CItem_Icon::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}

#include "Inventory.h"
#include "GameInstance.h"
#include "Item_Manager.h"


CInventory::CInventory(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CInventory::CInventory(const CInventory& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CInventory::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CInventory::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_Inven_INFO.vSize = { 918.f,207.f };
	m_Inven_INFO.vPos = { 0.f,170.f };
	m_bFrist_off_Item = false;
	m_pTransformCom->Set_Scale(m_Inven_INFO.vSize.x, m_Inven_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Inven_INFO.vPos.x, m_Inven_INFO.vPos.y, 0.f));
	 
	

	return S_OK;
}

void CInventory::Priority_Update(_float fTimeDelta)
{
}

void CInventory::Update(_float fTimeDelta)
{
	static float fElapsedTime = 0.0f;
	static bool bFirstUpdate = true;
	bool bInputReceived = false;

	if (GetAsyncKeyState('1') & 0x8000)
	{
		CItem_Manager::GetInstance()->Weapon_Equip(L"Claymore");
		bInputReceived = true;
	}
	if (GetAsyncKeyState('2') & 0x8000)
	{
		CItem_Manager::GetInstance()->Weapon_Equip(L"Axe");
		bInputReceived = true;
	}
	if (GetAsyncKeyState('3') & 0x8000)
	{
		CItem_Manager::GetInstance()->Weapon_Equip(L"ShotGun");
		bInputReceived = true;
	}
	if (GetAsyncKeyState('4') & 0x8000)
	{
		CItem_Manager::GetInstance()->Weapon_Equip(L"Magnum");
		bInputReceived = true;
	}
	if (GetAsyncKeyState('5') & 0x8000)
	{
		CItem_Manager::GetInstance()->Weapon_Equip(L"Staff");
		bInputReceived = true;
	}

	if (bInputReceived)
	{
		fElapsedTime = 0.0f; // 입력이 있으면 타이머 초기화
		m_bRender = true; // 입력이 있으면 인벤 창 보이기
	}
	else
	{
		fElapsedTime += fTimeDelta; // 입력이 없으면 타이머 증가
	}

	if (bFirstUpdate)
	{
		m_bRender = false; // 최초 업데이트 시 인벤 창 안보이게
		bFirstUpdate = false;
	}
	else if (fElapsedTime >= 2.0f) // 2초 동안 입력이 없으면 렌더 창 끄기
	{
		m_bRender = false;
	}
}

void CInventory::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
	if (!m_bFrist_off_Item)
	{
		CItem_Manager::GetInstance()->Weapon_UnEquip();
		m_bFrist_off_Item = true;
	}
}


HRESULT CInventory::Render()
{
	if (!m_bRender)
	{
		CItem_Manager::GetInstance()->Set_Inven_Render(false);
		return S_OK;
	}
	else
		CItem_Manager::GetInstance()->Set_Inven_Render(true);


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
	if (FAILED(m_pTextureCom->Bind_Resource(0))) // 현재 프레임 인덱스를 사용하여 텍스처 바인딩
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matOldView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOldProj);

}

HRESULT CInventory::Ready_Components()
{

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Inven"),
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


CInventory* CInventory::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CInventory* pInstance = new CInventory(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("인벤 UI 원본 생성 실패 ");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInventory::Clone(void* pArg)
{
	CInventory* pInstace = new CInventory(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{
		MSG_BOX("인벤 UI 복제 실패");
		Safe_Release(pInstace);
	}

	return pInstace;
}

void CInventory::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}

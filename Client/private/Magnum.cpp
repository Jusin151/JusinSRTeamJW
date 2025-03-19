#include "Magnum.h"
#include "GameInstance.h"
#include "CUI_Manager.h"
#include "Item_Manager.h"

CMagnum::CMagnum(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CRanged_Weapon(pGraphic_Device)
{
}

CMagnum::CMagnum(const CMagnum& Prototype)
	: CRanged_Weapon(Prototype),
	m_Magnum_INFO{ Prototype.m_Magnum_INFO }
{
}

HRESULT CMagnum::Initialize_Prototype()
{



	return S_OK;
}

HRESULT CMagnum::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pArg != nullptr)
		m_Magnum_INFO = *reinterpret_cast<Weapon_DESC*>(pArg);
	else
		return E_FAIL;


	m_pTransformCom->Set_Scale(m_Magnum_INFO.vSize.x, m_Magnum_INFO.vSize.y, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		_float3(m_Magnum_INFO.vPos.x, m_Magnum_INFO.vPos.y, 0.f));

	m_vInitialPos = m_Magnum_INFO.vPos;

	CItem_Manager::GetInstance()->Add_Weapon(L"Magnum", this);

	CItem_Icon::Icon_DESC Staff_Icon{};
	Staff_Icon.Icon_Image = 2;
	Staff_Icon.vPos = { -200.f,200.f };
	Staff_Icon.Icon_Image = 3;
	if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Icon"),
		LEVEL_GAMEPLAY, TEXT("Layer_Weapon_Icon_Magnum"),&Staff_Icon)))
		return E_FAIL;


	return S_OK;
}

void CMagnum::Priority_Update(_float fTimeDelta)
{
}

void CMagnum::Update(_float fTimeDelta)
{
	

	


	if (GetAsyncKeyState('W') & 0x8000)
	{
		t += speed;
	}
	else if (GetAsyncKeyState('A') & 0x8000)
	{
		t += speed;
	}
	else if (GetAsyncKeyState('D') & 0x8000)
	{
		t += speed;
	}
	else if (GetAsyncKeyState('S') & 0x8000)
	{
		t += speed;
	}

	float v = 20.0f;  // 폭을 설정 하는변수
	_float3 vNewPos;
	vNewPos.x = m_vInitialPos.x + (1 + v * cosf(t / 2)) * cosf(t);
	vNewPos.y = m_vInitialPos.y + (1 + v * cosf(t / 2)) * sinf(t);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPos);
	
	Attack(fTimeDelta);
	return;
}
void CMagnum::Attack(_float fTimeDelta)
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		m_bIsAnimating = true;
		m_iCurrentFrame = 0;
		m_fElapsedTime = 0.0f;
	}
	if (m_bIsAnimating)
	{
		m_fElapsedTime += fTimeDelta;
		if (m_fElapsedTime >= 0.02f)
		{
			m_fElapsedTime = 0.0f;
			if (m_iCurrentFrame < 4) // 몇장까지인지
			{
				m_iCurrentFrame++;
			}
			else
			{
				m_bIsAnimating = false;
				m_iCurrentFrame = 0;
			}
		}
	}
}

void CMagnum::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this)))
		return;
}

HRESULT CMagnum::Render()
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
	if (FAILED(m_pTextureCom->Bind_Resource(m_iCurrentFrame)))
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

HRESULT CMagnum::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Magnum"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBufferm"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	CTransform::TRANSFORM_DESC tDesc{ 10.f,D3DXToRadian(90.f) };
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &tDesc)))
		return E_FAIL;

	return S_OK;
}

CMagnum* CMagnum::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMagnum* pInstance = new CMagnum(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{

		MSG_BOX("매그넘 UI 원본 생성 실패 ");

		Safe_Release(pInstance);
	}


	return pInstance;
}

CGameObject* CMagnum::Clone(void* pArg)
{
	CMagnum* pInstace = new CMagnum(*this);

	if (FAILED(pInstace->Initialize(pArg)))
	{

		MSG_BOX("매그넘 UI 복제 실패");

		Safe_Release(pInstace);
	}

	return pInstace;
}

void CMagnum::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}

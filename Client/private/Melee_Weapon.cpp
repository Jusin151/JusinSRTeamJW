#include "Melee_Weapon.h"
#include "GameInstance.h"

CMelee_Weapon::CMelee_Weapon(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CWeapon_Base{pGraphic_Device}
{
}

CMelee_Weapon::CMelee_Weapon(const CMelee_Weapon& Prototype)
	: CWeapon_Base(Prototype)
{
}

HRESULT CMelee_Weapon::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CMelee_Weapon::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CMelee_Weapon::Priority_Update(_float fTimeDelta)
{
}

void CMelee_Weapon::Update(_float fTimeDelta)
{
	Attack();
	if (m_bIsAnimating)
	{
		m_fElapsedTime += fTimeDelta;
		if (m_fElapsedTime >= 0.04f)
		{
			m_fElapsedTime = 0.0f;
			if (m_iCurrentFrame < 13)
			{
				m_iCurrentFrame++;
			}
			else
			{
				m_bIsAnimating = false;
			}
		}
	}
}

void CMelee_Weapon::Late_Update(_float fTimeDelta)
{
}

HRESULT CMelee_Weapon::Render()
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

HRESULT CMelee_Weapon::Ready_Components()
{
	return E_NOTIMPL;
}

void CMelee_Weapon::Free()
{
	__super::Free();
}

CGameObject* CMelee_Weapon::Clone(void* pArg)
{
	return nullptr;
}


void CMelee_Weapon::Attack()
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		m_bIsAnimating = true;
		m_iCurrentFrame = 0;
		m_fElapsedTime = 0.0f;
	}
}

#include "Ranged_Weapon.h"
#include "GameInstance.h"
#include "Collider.h"
#include "PickingSys.h"

CRanged_Weapon::CRanged_Weapon(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CWeapon_Base{ pGraphic_Device }
{
}

CRanged_Weapon::CRanged_Weapon(const CRanged_Weapon& Prototype)
	: CWeapon_Base(Prototype)
{
}
void CRanged_Weapon::Priority_Update(_float fTimeDelta)
{
	
}

void CRanged_Weapon::Update(_float fTimeDelta)
{
	Attack(fTimeDelta);

}

void CRanged_Weapon::Late_Update(_float fTimeDelta)
{
}

HRESULT CRanged_Weapon::Render()
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

HRESULT CRanged_Weapon::Ready_Components()
{
	return E_NOTIMPL;
}

HRESULT CRanged_Weapon::Ready_Picking()
{
	m_pPickingSys = CPickingSys::Get_Instance();


	return S_OK;
}

HRESULT CRanged_Weapon::Picking_Object()
{

 	m_pPickingSys->Update();

	static CGameObject* dragObject = nullptr;

	auto ColliderVec = m_pGameInstance->Get_Colliders();
	if (!(GetKeyState(VK_LBUTTON) & 0x8000))
	{
		dragObject = nullptr;
	}
	else
	{
		for (auto& colliderList : ColliderVec)
		{
			for (auto& collider : colliderList)
			{
				if (m_pPickingSys->Ray_Intersection(collider))
				{

					if (collider->Get_Owner()->Get_Tag() == L"Layer_Playre")
						continue;

					if (collider->Get_Owner()->Get_Tag() == L"Layer_Wall")
						m_bWall = true;
					else
						m_bWall = false;

					if (collider->Get_Owner()->Get_Tag() == L"Layer_Crocman")
						m_bMonster = true;
					else
						m_bMonster = false;
					break;
				}

			}
		}

	}



	return S_OK;
}

void CRanged_Weapon::Free()
{
	__super::Free();
}

CGameObject* CRanged_Weapon::Clone(void* pArg)
{
	return nullptr;
}


void CRanged_Weapon::Attack(_float fTimeDelta)
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




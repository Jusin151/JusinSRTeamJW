#include "Weapon_Base.h"

#include "GameInstance.h"
#include "Melee_Weapon.h"
#include "Melee_Weapon.h"
#include "Melee_Weapon.h"


CWeapon_Base::CWeapon_Base(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CWeapon_Base::CWeapon_Base(const CWeapon_Base& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CWeapon_Base::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Base::Initialize(void* pArg)
{
	return S_OK;
}

void CWeapon_Base::Update(_float fTimeDelta)
{

}

HRESULT CWeapon_Base::Render()
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
	if (FAILED(m_pTextureCom->Bind_Resource(0)))
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


void CWeapon_Base::Priority_Update(_float fTimeDelta)
{
}

void CWeapon_Base::Late_Update(_float fTimeDelta)
{
}

void CWeapon_Base::Free()
{
	__super::Free();
}

CGameObject* CWeapon_Base::Clone(void* pArg)
{
	return nullptr;
}





HRESULT CWeapon_Base::Ready_Components()
{

	return S_OK;
}

#include "CUI_Base.h"

#include "GameInstance.h"


CUI_Base::CUI_Base(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{pGraphic_Device}
{
}

CUI_Base::CUI_Base(const CUI_Base& Prototype)
 : CGameObject(Prototype)
{
}

HRESULT CUI_Base::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Base::Initialize(void* pArg)
{
	return S_OK;
}

void CUI_Base::Update(_float fTimeDelta)
{

}

HRESULT CUI_Base::Render()
{

	return S_OK;
}


void CUI_Base::Priority_Update(_float fTimeDelta)
{
}

void CUI_Base::Late_Update(_float fTimeDelta)
{
}

void CUI_Base::Free()
{
	__super::Free();
}

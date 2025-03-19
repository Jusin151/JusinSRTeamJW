#include "LandObject.h"

#include "GameInstance.h"


CLandObject::CLandObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CLandObject::CLandObject(const CLandObject& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CLandObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLandObject::Initialize(void* pArg)
{
	LANDOBJECT_DESC* pDesc = static_cast<LANDOBJECT_DESC*>(pArg);

	/*m_pTargetBuffer = static_cast<CVIBuffer*>(m_pGameInstance->Get_Component(pDesc->iLevelIndex, pDesc->strLayerTag, pDesc->strComponentTag, pDesc->iIndex));*/

	if (nullptr == m_pTargetBuffer)
		return E_FAIL;	

	return S_OK;
}

void CLandObject::Priority_Update(_float fTimeDelta)
{
}

void CLandObject::Update(_float fTimeDelta)
{
}

void CLandObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CLandObject::Render()
{
	return S_OK;
}


void CLandObject::SetUp_HeightPosition(CTransform* pTransform, _float fOffset)
{
	_float3		vPosition = pTransform->Get_State(CTransform::STATE_POSITION);
	vPosition = m_pTargetBuffer->Compute_HeightPosition(vPosition);

	vPosition.y += fOffset;

	pTransform->Set_State(CTransform::STATE_POSITION, vPosition);
}

void CLandObject::Free()
{
	__super::Free();

	Safe_Release(m_pTargetBuffer);
}

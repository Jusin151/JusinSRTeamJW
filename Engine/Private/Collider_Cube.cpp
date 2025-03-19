#include "Collider_Cube.h"
#include "GameObject.h"

CCollider_Cube::CCollider_Cube(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CCollider(pGraphic_Device)
{
}

CCollider_Cube::CCollider_Cube(const CCollider_Cube& Prototype)
	:CCollider(Prototype)
{
}

HRESULT CCollider_Cube::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCollider_Cube::Initialize(void* pArg)
{
	COL_CUBE_DESC* tDesc;

	tDesc = static_cast<COL_CUBE_DESC*>(pArg);
	
	m_fLocalPos = tDesc->fLocalPos;
	m_fScale = tDesc->fScale;
	m_pOwner = tDesc->pOwner;
	m_eType = tDesc->eType;
	
	m_tDesc.fRadius = m_fScale.LengthSq() / 4.f;

	D3DXMatrixIdentity(&m_WorldMatrix);
	return S_OK;
}

HRESULT CCollider_Cube::Render()
{
	return S_OK;
}


HRESULT CCollider_Cube::Update_Collider(const _wstring& strLayerTag)
{

	if (nullptr == m_pOwner)
		return E_FAIL;



	// owner 가 가지고 있는 월드 행렬을 이용해서 collider의 월드 행렬을 만든다
	CTransform* pTransfrom = static_cast<CTransform*>(m_pOwner->Get_Component(strLayerTag));



	_float3 fRight = pTransfrom->Get_State(CTransform::STATE_RIGHT).GetNormalized();
	_float3 fUp = pTransfrom->Get_State(CTransform::STATE_UP).GetNormalized();
	_float3 fLook = pTransfrom->Get_State(CTransform::STATE_LOOK).GetNormalized();
	_float3 fObjectPos = pTransfrom->Get_State(CTransform::STATE_POSITION);

	Set_State(CTransform::STATE_RIGHT, fRight * m_fScale.x);
	Set_State(CTransform::STATE_UP, fUp * m_fScale.y);
	Set_State(CTransform::STATE_LOOK, fLook * m_fScale.z);
	Set_State(CTransform::STATE_POSITION, fObjectPos);


	D3DXVec3TransformCoord(&m_fPos, &m_fLocalPos, &m_WorldMatrix);
	Set_State(CTransform::STATE_POSITION,  m_fPos);

	// 축과 점 위치를 월드 행렬에 맞게 고친다

	m_tDesc.vecIndices.clear();

	m_tDesc.fAxisX = Get_State(CTransform::STATE_RIGHT);
	m_tDesc.fAxisY = Get_State(CTransform::STATE_UP);
	m_tDesc.fAxisZ = Get_State(CTransform::STATE_LOOK);
	m_tDesc.fPos = Get_State(CTransform::STATE_POSITION);

	_float3 axis[3] = { m_tDesc.fAxisX, m_tDesc.fAxisY, m_tDesc.fAxisZ };
	for (int i = 0; i < 8; ++i)
	{
		_float3 offset =
			((i & 1) ? axis[0] : -axis[0]) * 0.5f +
			((i & 2) ? axis[1] : -axis[1]) * 0.5f +
			((i & 4) ? axis[2] : -axis[2]) * 0.5f;
		m_tDesc.vecIndices.push_back(m_tDesc.fPos + offset);
	}

	m_tDesc.fRadius = m_fScale.LengthSq() / 4.f;

	return S_OK;

}

CCollider_Cube* CCollider_Cube::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCollider_Cube* pInstance = new CCollider_Cube(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCollider_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider_Cube::Clone(void* pArg)
{
	CCollider_Cube* pInstance = new CCollider_Cube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCollider_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Cube::Free()
{
	__super::Free();
}

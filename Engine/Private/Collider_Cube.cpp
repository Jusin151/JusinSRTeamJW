#include "Collider_Cube.h"
#include "VIBuffer_Cube.h"
#include "GameObject.h"
#include "CollisionObject.h"

CCollider_Cube::CCollider_Cube(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CCollider(pGraphic_Device)
{
}

CCollider_Cube::CCollider_Cube(const CCollider_Cube& Prototype)
	:CCollider{ Prototype }
	, m_tDesc { Prototype.m_tDesc }
{
}

HRESULT CCollider_Cube::Initialize_Prototype()
{
 	m_iNumVertices = 8;
	m_iVertexStride = sizeof(VTXPOSCOL);
	m_iFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

	m_iNumLine = 12;
	m_iIndexStride = 2;
	m_iNumIndices = 2 * m_iNumLine;
	m_eIndexFormat = D3DFMT_INDEX16;

#pragma region VERTEX_BUFFER
	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	VTXPOSCOL* pVertices = { nullptr };


	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, reinterpret_cast<void**>(&pVertices), 0);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		pVertices[i].vColor = D3DCOLOR_COLORVALUE( 0.0f, 1.0f, 0.0f, 1.0f );
	}
	m_pVB->Unlock();
#pragma endregion

#pragma region INDEX_BUFFER
	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	_ushort* pIndices = { nullptr };

	m_pIB->Lock(0, 0, reinterpret_cast<void**>(&pIndices), 0);

	// Back Face
	pIndices[0] = 0, pIndices[1] = 1;
	pIndices[2] = 1, pIndices[3] = 2;
	pIndices[4] = 2, pIndices[5] = 3;
	pIndices[6] = 3, pIndices[7] = 0;

	// Front Face
	pIndices[8]  = 4, pIndices[9]  = 5;
	pIndices[10] = 5, pIndices[11] = 6;
	pIndices[12] = 6, pIndices[13] = 7;
	pIndices[14] = 7, pIndices[15] = 4;

	// Connecting Edges
	pIndices[16] = 0, pIndices[17] = 4;
	pIndices[18] = 1, pIndices[19] = 5;
	pIndices[20] = 2, pIndices[21] = 6;
	pIndices[22] = 3, pIndices[23] = 7;


 	m_pIB->Unlock();

#pragma endregion
	return S_OK;
}

HRESULT CCollider_Cube::Initialize(void* pArg)
{
	COL_CUBE_DESC* tDesc;

	tDesc = static_cast<COL_CUBE_DESC*>(pArg);
	
	m_fLocalPos = tDesc->fLocalPos;
	m_fScale = tDesc->fScale;
	m_pOwner = tDesc->pOwner;
	
	m_tDesc.fRadius = m_fScale.LengthSq() / 4.f;

	D3DXMatrixIdentity(&m_WorldMatrix);
	return S_OK;
}

HRESULT CCollider_Cube::Render()
{
	D3DMATERIAL9 lineMaterial;
	ZeroMemory(&lineMaterial, sizeof(D3DMATERIAL9));
	lineMaterial.Diffuse.r = 0.0f; // R
	lineMaterial.Diffuse.g = 1.0f; // G
	lineMaterial.Diffuse.b = 0.0f; // B
	lineMaterial.Diffuse.a = 1.0f; // Alpha (블렌딩 시 사용될 투명도)
	// Emissive를 사용해도 조명이 꺼져있으면 비슷한 효과를 낼 수 있습니다.
	lineMaterial.Emissive.r = 0.0f; lineMaterial.Emissive.g = 1.0f; lineMaterial.Emissive.b = 0.0f; lineMaterial.Emissive.a = 1.0f;
	m_pGraphic_Device->SetMaterial(&lineMaterial);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//
	////색깔 덧셈
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	//
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE); // 와이어가 Z값을 덮어쓰지 않도록
	m_pGraphic_Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL); // 동일한 깊이에서도 그려지도록
	//
	//
	// 3. Z-파이팅 방지: 깊이 바이어스(Depth Bias) 적용 (중요!)
	//    와이어프레임이 텍스처 표면과 거의 같은 깊이에 있어 깜빡이는 현상(Z-fighting)을 방지
	//    값을 약간 조정하여 와이어프레임이 미세하게 더 가깝게 그려지도록 함
	float fDepthBias = -0.00001f; // 값은 실험을 통해 조정 필요
	float fSlopeScaleBias = -1.0f; // 표면 기울기에 따른 바이어스 조정
	m_pGraphic_Device->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&fDepthBias);
	m_pGraphic_Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&fSlopeScaleBias);
	m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_iVertexStride);
	m_pGraphic_Device->SetIndices(m_pIB);
	m_pGraphic_Device->SetFVF(m_iFVF);
	m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, m_iNumVertices, 0, m_iNumLine);
	D3DMATERIAL9 defaultMaterial;
	ZeroMemory(&defaultMaterial, sizeof(D3DMATERIAL9));
	defaultMaterial.Diffuse.r = 1.0f; // R  
	defaultMaterial.Diffuse.g = 1.0f; // G  
	defaultMaterial.Diffuse.b = 1.0f; // B  
	defaultMaterial.Diffuse.a = 1.0f; // Alpha (블렌딩 시 사용될 투명도)  
	// Emissive를 사용해도 조명이 꺼져있으면 비슷한 효과를 낼 수 있습니다.  
	defaultMaterial.Emissive = { 1.0f, 1.0f, 1.0f, 1.0f };  
	m_pGraphic_Device->SetMaterial(&defaultMaterial);  
	fDepthBias = 0.0f;
	fSlopeScaleBias = 0.0f;
	m_pGraphic_Device->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&fDepthBias);
	m_pGraphic_Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&fSlopeScaleBias);
	m_pGraphic_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE); // 와이어가 Z값을 덮어쓰지 않도록
	//m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	
	return S_OK;
}


HRESULT CCollider_Cube::Update_Collider(const _wstring& strLayerTag, _float3 fScale)
{

	if (nullptr == m_pOwner)
		return E_FAIL;

	// owner 가 가지고 있는 월드 행렬을 이용해서 collider의 월드 행렬을 만든다
	CTransform* pTransfrom = static_cast<CTransform*>(m_pOwner->Get_Component(strLayerTag));

	_float3 fRight = pTransfrom->Get_State(CTransform::STATE_RIGHT).GetNormalized();
	_float3 fUp = pTransfrom->Get_State(CTransform::STATE_UP).GetNormalized();
	_float3 fLook = pTransfrom->Get_State(CTransform::STATE_LOOK).GetNormalized();
	_float3 fObjectPos = pTransfrom->Get_State(CTransform::STATE_POSITION);

	Set_State(CTransform::STATE_RIGHT, fRight * fScale.x);
	Set_State(CTransform::STATE_UP, fUp * fScale.y);
	Set_State(CTransform::STATE_LOOK, fLook * fScale.z);
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
	_float3 offset = {};
	for (int i = 0; i < 8; ++i)
	{
		    offset =
			((i & 1) ? axis[0] : -axis[0]) * 0.5f +
			((i & 2) ? axis[1] : -axis[1]) * 0.5f +
			((i & 4) ? axis[2] : -axis[2]) * 0.5f;
		m_tDesc.vecIndices.push_back(m_tDesc.fPos + offset);
	}

	m_tDesc.fRadius = offset.Length();

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

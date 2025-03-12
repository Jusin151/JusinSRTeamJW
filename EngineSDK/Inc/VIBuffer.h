#pragma once

/* V : Vertex */
/* I : Index */
/* Buffer : 메모리공간 */
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer(const CVIBuffer& Prototype);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render();

public:
	HRESULT Bind_Buffers();

protected:
	/*  정점 배열을 가지고 있는 컴 객체. */
	LPDIRECT3DVERTEXBUFFER9				m_pVB = { nullptr };
	LPDIRECT3DINDEXBUFFER9				m_pIB = { nullptr };

	_uint								m_iNumVertices = { };
	_uint								m_iVertexStride = { };
	_uint								m_iFVF = {};
	_uint								m_iNumPritimive = {};

	_uint								m_iNumIndices = {};
	_uint								m_iIndexStride = {};
	D3DFORMAT							m_eIndexFormat = {};

protected:
	HRESULT Create_VertexBuffer();
	HRESULT Create_IndexBuffer();

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
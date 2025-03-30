#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_TexturedCube final : public CVIBuffer
{
private:
	CVIBuffer_TexturedCube(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_TexturedCube(const CVIBuffer_TexturedCube& Prototype);
	virtual ~CVIBuffer_TexturedCube() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CVIBuffer_TexturedCube* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
END
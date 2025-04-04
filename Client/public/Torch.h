#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CMaterial;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CTorch final : public CGameObject
{
private:
	CTorch(LPDIRECT3DDEVICE9 pGraphic_Device);
	CTorch(const CTorch& Prototype);
	virtual ~CTorch() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
private:
	HRESULT Ready_Components();
public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
public:
	virtual HRESULT Pre_Render();
	virtual HRESULT Render() override;
	virtual HRESULT Post_Render();
public:
	virtual void Reset() {}; // 차후에 오브젝트 풀링때 SetActive가 true가 될 때 기본정보 다시 셋팅을 위한 함수

private:
	CShader*			m_pShaderCom	= { nullptr };
	CTexture*			m_pTextureCom	= { nullptr };
	CMaterial*			m_pMaterialCom	= { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom	= { nullptr };

public:
	static CTorch* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CTorch* Clone(void* pArg) override;
	virtual void Free() override;
public:
	virtual json Serialize() override;
	virtual void Deserialize(const json& j) override;
};
END
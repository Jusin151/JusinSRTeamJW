#pragma once
#include "Client_Defines.h"
#include "Transform.h"
#include "GameObject.h"
#include "Serializable.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer;
class CCollider;
END

BEGIN(Client)
class CStructure : public CGameObject
{
	friend class CMyImGui;
	typedef struct tagStructureDesc : public CTransform::TRANSFORM_DESC
	{
		const _tchar* stCollProtoTag;
		const _tchar* stVIBuffer;
		const _tchar* stTextureTag;
		_wstring stTexturePath;
	}STRUCTURE_DESC;

private:
	CStructure(LPDIRECT3DDEVICE9 pGraphic_Device);
	CStructure(const CStructure& Prototype);
	virtual ~CStructure() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;
	HRESULT On_Collision(_float fTimeDelta);

private:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer* m_pVIBufferCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	_bool m_bIsCubeCollider = { false };
	STRUCTURE_DESC m_tStructure_Desc;
	_wstring m_strVIBuffer;
	_wstring m_strCollProtoTag;
	_wstring m_strTextureTag;

private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

	HRESULT Ready_Components();


public:
	static CStructure* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();

	virtual json Serialize() override;
	virtual void Deserialize(const json& j) override;
};
END

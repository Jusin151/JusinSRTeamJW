#pragma once

#include "Client_Defines.h"
#include "CollisionObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CMaterial;
class CTransform;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)

class CProjectile_Base abstract : public CCollisionObject
{
public:
	typedef struct tagProjectileDesc {
		_float3 vPos;
		_float3 vDir;
		_float fSpeed;
		
	}PROJ_DESC;

protected:
	CProjectile_Base(LPDIRECT3DDEVICE9 pGraphic_Device);
	CProjectile_Base(const CProjectile_Base& Prototype);
	virtual ~CProjectile_Base() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;
	virtual HRESULT On_Collision(CCollisionObject* other) { return S_OK; }

	// collider,texture는 자식에서 추가
	HRESULT Ready_Components();

	
protected:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CMaterial*		m_pMaterialCom = { nullptr };
	CTransform*		m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	// 투사체 따라서 다를 수 있어서 원하는거로 자식에서 설정하면 될듯?
	CCollider*		m_pColliderCom = { nullptr };

	_float3 m_vPos = {};
	_float3 m_vDir = {};
	_float m_fSpeed = {};

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free();
};


END
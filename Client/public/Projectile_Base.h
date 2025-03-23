#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)

class CProjectile_Base abstract : public CGameObject
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
	virtual HRESULT On_Collision(_float fTimeDelta) PURE;

	// collider,texture는 자식에서 추가
	HRESULT Ready_Components();

	
protected:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	// 투사체 따라서 다를 수 있어서 원하는거로 자식에서 설정하면 될듯?
	CCollider* m_pColliderCom = { nullptr };

	_float3 m_vPos;
	_float3 m_vDir;
	_float m_fSpeed;

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free();
};


END
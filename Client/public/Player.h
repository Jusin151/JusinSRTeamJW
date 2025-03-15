#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Cube;
class CCollider_Sphere;
class CCollider_Cube;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
private:
	CPlayer(LPDIRECT3DDEVICE9 pGraphic_Device);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;
	HRESULT On_Collision();

private:
	CTexture*				m_pTextureCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Cube*			m_pVIBufferCom = { nullptr };
	CCollider_Sphere*		m_pColliderCom = { nullptr };

private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

	HRESULT Ready_Components();


public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};


END
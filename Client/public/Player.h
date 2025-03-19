#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Weapon_Base.h"



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
	void Move(_float fTimeDelta);
	CTransform* Get_TransForm() { return m_pTransformCom; }
private:
	CTexture*				m_pTextureCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	CVIBuffer_Cube*			m_pVIBufferCom = { nullptr };
private:
	POINT m_ptOldMousePos = {};
	_float m_fMouseSensor = {};
private:
	CGameObject* m_pCamera = nullptr; 
	CCollider_Cube*			m_pColliderCom = { nullptr };
private: // 인벤관련
	void Inven_Update(_float fTimeDelta);
	_bool m_bInven_Render_State{};
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
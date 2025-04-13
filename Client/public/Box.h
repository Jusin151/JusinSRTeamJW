#pragma once
#include "Deco_Base.h"

class CBox : public CDeco_Base
{
private:
	CBox(LPDIRECT3DDEVICE9 pGraphic_Device);
	CBox(const CBox& Prototype);
	virtual ~CBox() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
private:
	// texturecom는 각 monster에서 추가
	HRESULT Ready_Components();

public:
	virtual void Priority_Update(_float fTimeDelta)override;
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;

public:
	virtual HRESULT Render()override;
	virtual HRESULT On_Collision(CCollisionObject* other);



private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();
	virtual void Select_State()override;

	void	Smart_Drop();

private:


public:
	static CBox* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();


};


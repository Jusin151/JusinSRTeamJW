#pragma once
#include "Deco_Base.h"

class CStatue : public CDeco_Base
{
private:
	CStatue(LPDIRECT3DDEVICE9 pGraphic_Device);
	CStatue(const CStatue& Prototype);
	virtual ~CStatue() = default;

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

private:
	

public:
	static CStatue* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();


};


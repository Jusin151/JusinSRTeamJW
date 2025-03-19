#pragma once

#include "Monster_Base.h"

class CAnubis : public CMonster_Base 
{
private:
	CAnubis(LPDIRECT3DDEVICE9 pGraphic_Device);
	CAnubis(const CAnubis& Prototype);
	virtual ~CAnubis() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	// 마지막에 플레이어 방향으로 바라보도록 함
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;


public:
	virtual HRESULT On_Collision(_float fTimeDelta) override;
	// 일단 쫓아가는 패턴만
	virtual void Select_Pattern(_float fTimeDelta) override;


private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

	// 텍스처 추가 
	HRESULT Ready_Components();


private:
	
	
public:
	static CAnubis* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();
};


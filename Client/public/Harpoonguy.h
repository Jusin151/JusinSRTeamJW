#pragma once

#include "Monster_Base.h"

class CHarpoonguy : public CMonster_Base
{
private:
	CHarpoonguy(LPDIRECT3DDEVICE9 pGraphic_Device);
	CHarpoonguy(const CHarpoonguy& Prototype);
	virtual ~CHarpoonguy() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Priority_Update(_float fTimeDelta)override;
	// 마지막에 플레이어 방향으로 바라보도록 함
	virtual void Update(_float fTimeDelta)override;
	virtual void Late_Update(_float fTimeDelta)override;
	virtual HRESULT Render()override;

	virtual void Deserialize(const json& j) override;

public:
	virtual HRESULT On_Collision(_float fTimeDelta) override;
	// 일정 거리 까지 쫓아가거나, 일정 거리 이내면 콜라이더 생성해서 공격하도록
	virtual void Select_Pattern(_float fTimeDelta) override;

	void Chasing(_float fTimeDelta);
	void Shooting(_float fTimeDelta);

	// 각 상태에 맞게 프레임을 설정함
	void Select_Frame(_float fTimeDelta);

private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

	// 텍스처 추가 
	HRESULT Ready_Components();


private:

public:
	static CHarpoonguy* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();
};


#pragma once

#include "Monster_Base.h"

class CGlacier : public CMonster_Base
{
public:
	enum HPSTATE {HP_MAX, HP_HURT, HP_VERYHURT};

private:
	CGlacier(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGlacier(const CGlacier& Prototype);
	virtual ~CGlacier() = default;

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
	virtual HRESULT On_Collision(CCollisionObject* other) override;
	// 일정 거리 까지 쫓아가거나, 일정 거리 이내면 콜라이더 생성해서 공격하도록
	virtual void Select_Pattern(_float fTimeDelta) override;

	// hp 확인하면서 HPSTATE 변경
	void Select_HpState();
	// 몬스터 기준으로 플레이어가 왼쪽에 있는지 오른쪽에 있는지 확인. 0이면 look, 0 초과하면 오른쪽, 0 미만이면 왼쪽
	_float Check_Direction();

	void Shooting(_float fTimeDelta);

	// 각 상태에 맞게 프레임을 설정함
	void Select_Frame(_float fTimeDelta);

private:
	HRESULT SetUp_RenderState();
	HRESULT Release_RenderState();

	// 텍스처 추가 
	HRESULT Ready_Components();


private:
	HPSTATE m_eHpState = { HP_MAX };
	_uint m_iMaxHp = {0};
	_bool m_bRight = { false };

public:
	static CGlacier* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject* Clone(void* pArg) override;
	virtual void Free();
};


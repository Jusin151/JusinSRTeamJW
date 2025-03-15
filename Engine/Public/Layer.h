#pragma once

#include "Base.h"

/* 객체들을 모아놓는다. */

BEGIN(Engine)

class CLayer final : public CBase
{
	friend class CMyImGui;
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
private:
	list<class CGameObject*>			m_GameObjects;
	class CGameInstance* m_pGameInstance;
	_uint m_iPrototypeLevelIndex; // 자기 자신이 어디에 속한 레벨의 레이어인지 알기 위함
public:
	static CLayer* Create(_uint iPrototypeLevelIndex);
	virtual void Free();
};

END
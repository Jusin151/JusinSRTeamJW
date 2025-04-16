#pragma once

#include "Base.h"

/* 객체들을 모아놓는다. */

BEGIN(Engine)

class CLayer final : public CBase
{
	friend class CMyImGui;
	friend class CEditor;
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_Component(const _wstring& strComponentTag, _uint iIndex);

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);

	// 일단 제일 앞에 거 가져오도록. 나중에 생각...
	class CGameObject* Find_Object();
	class CGameObject* Find_Last_Object();
	HRESULT Remove_GameObject(class CGameObject* pGameObject);	
	list<CGameObject*> Get_LayerObjects();
private:
	list<class CGameObject*>			m_GameObjects;
	class CGameInstance* m_pGameInstance;
	_uint m_iPrototypeLevelIndex; // 자기 자신이 어디에 속한 레벨의 레이어인지 알기 위함
public:
	static CLayer* Create(_uint iPrototypeLevelIndex);
	virtual void Free();

	const list<CGameObject*>& Get_GameObjects() const { return m_GameObjects; }


};

END
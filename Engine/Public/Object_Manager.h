#pragma once

#include "Base.h"

/* 레벨별로 구분하여 사본객체를 보관한다. */
/* 리스트들로 보관한다. 리스트 == 객체들 == 내가 나누고 싶은 기준에따라 그룹짓는다. */

BEGIN(Engine)


class CObject_Manager final : public CBase
{
	friend class CEditor;
	friend class CMyImGui;
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent* Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex);

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, void* pArg);
	class CGameObject* Add_GameObject_FromPool(_uint iPrototypeLevelIndex, _uint iLevelIndex,const _wstring& strLayerTag,void* pArg = nullptr); // 풀 매니저에서 뽑아쓸 때 쓰는 add
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	void Clear(_uint iLevelIndex);
	class CGameObject* Find_Object(_uint iLevelIndex, const _wstring& strLayerTag);
	class CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag);
	class CGameObject* Find_Last_Object(_uint iLevelIndex, const _wstring& strLayerTag);
	HRESULT Remove_Object(_uint iLevelIndex, const _wstring& strLayerTag, class CGameObject* pGameObject);
	void Open_UI(_uint iLevelIndex, _bool bOpenUI) { m_bOpendUI = bOpenUI; m_iLevelIndex = iLevelIndex; }
private:	
	_uint								m_iNumLevels = {};
	vector<map<const _wstring, class CLayer*>>m_pLayers;
	class CGameInstance*				m_pGameInstance = { nullptr };
	_bool m_bOpendUI = { true };
	_uint m_iLevelIndex = -1;

private:
	class CLayer* Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);

public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END
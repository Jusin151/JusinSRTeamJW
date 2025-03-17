#pragma once

#include "Base.h"
#include "Serializable.h"
BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase ,public ISerializable
{
	friend class CMyImGui;
protected:
	CGameObject(LPDIRECT3DDEVICE9 pGraphic_Device);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	/* 원형이 호출하는 함수. 오래걸릴 수 있는 초기화작업 */
	virtual HRESULT Initialize_Prototype();
	/* 사본이 호출하는 함수. */
	virtual HRESULT Initialize(void* pArg);

	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Reset() {}; // 차후에 오브젝트 풀링때 SetActive가 true가 될 때 기본정보 다시 셋팅을 위한 함수

public:
	bool IsActive() const { return m_bIsActive; }
	void SetActive(_bool bIsActive);

	void Set_Tag(const _wstring& strLayerTag) { m_strLayerTag = strLayerTag; } // 오브젝트 태그 설정 
	const _wstring& Get_Tag() const { return m_strLayerTag; } // 오브젝트 태그 가져오기

	void Set_FromPool(_bool bFromPool) { m_bFromPool = bFromPool; } // 오브젝트 풀에서 가져온건지 설정
	bool Is_FromPool() const { return m_bFromPool; } 

	class CComponent* Get_Component(const wstring& strComponentTag) { 
		auto it = m_Components.find(strComponentTag);
		return it != m_Components.end() ? it->second : nullptr;
	}

public:
	virtual json Serialize() override;
	virtual void Deserialize(const json& j) override;

protected:
	LPDIRECT3DDEVICE9				m_pGraphic_Device = { nullptr };
	class CGameInstance*			m_pGameInstance = { nullptr };

protected:
	map<const _wstring, class CComponent*>	m_Components;
	_bool m_bIsActive = { false }; // 풀링을 위한 active 변수
	_bool m_bFromPool = { false }; // 게임 종료시에 반환 때 풀링에서 등록된 오브젝트인지 확인하기 위함
	_wstring m_strLayerTag;
	_wstring m_strObjectName;

protected:
	HRESULT Add_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);
	class CComponent* Find(const _wstring strComponentTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free();
};

END
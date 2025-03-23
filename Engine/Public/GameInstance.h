#pragma once

/* 엔진과 클라이언트 간의 링크의 역활을 수행한다. */
/* 엔진 내에 존재하는 유일한 싱글톤 클래스이다. */
/* 엔진 개발자가 클라개밫자에게 보여주고싶은 함수를 ... */
#include "Renderer.h"
#include "Prototype_Manager.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)

private:
	friend class CEditor;
	friend class CMyImGui;
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ LPDIRECT3DDEVICE9* ppOut);
	void Update_Engine(_float fTimeDelta);
	HRESULT Draw();	
	void Clear(_uint iLevelIndex);

public:
#pragma region LEVEL_MANAGER
	HRESULT Change_Level(_uint iLevelIndex, class CLevel* pNewLevel);
#pragma endregion

#pragma region PROTOTYPE_MANAGER
	HRESULT Add_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, class CBase* pPrototype);
	CBase*	Clone_Prototype(PROTOTYPE ePrototypeType, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	HRESULT Find_Prototype(const _wstring& strPrototypeTag);
#pragma endregion

#pragma region OBJECT_MANAGER
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
	HRESULT Add_GameObject_FromPool(_uint iPrototypeLevelIndex, _uint iLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);
	CGameObject* Find_Object(_uint iLevelIndex, const _wstring& strLayerTag);
	CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag);
	CGameObject* Find_Last_Object(_uint iLevelIndex, const _wstring& strLayerTag);
	/*CComponent* Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);*/
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderGroup(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Add_Light(class CLight* pLight);
#pragma endregion

#pragma region TIMER_MANAGER
	_float	Get_TimeDelta(const _wstring& strTimerTag);
	HRESULT Add_Timer(const _wstring& strTimerTag);
	void	Update_Timer(const _wstring& strTimerTag);
#pragma endregion

#pragma region SOUND_MANAGER

#pragma endregion

#pragma region POOL_MANAGER
	class CGameObject* Acquire_Object(_uint iPrototypeLevelIndex, const _wstring& strLayerTag,void* pArg = nullptr);
	// 풀에 객체 반환하기
	HRESULT Return_Object(_uint iPrototypeLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject);
	// 어디 레벨에 있는지, 어떤 이름으로 있었는지, 나중에 불러올 때 사용할 태그, 몇개 만들지, 던져줘야할 인자
	HRESULT Reserve_Pool(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strLayerTag, _uint iCount, void* pArg = nullptr);
#pragma endregion


#pragma region Collider_Manager

	HRESULT Add_Collider(COLLIDERGROUP eGroup, class CCollider* Collider);
	vector<list<class CCollider*>> Get_Colliders();

#pragma endregion

#pragma region Graphic_Device
	void Change_ClearColor(_float4 rgba);
#pragma endregion



#pragma region FONT_MANAGER
	HRESULT Add_Font(const _wstring& strFontTag, const _wstring& strFontFilePath);
	HRESULT Render_Font(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition, _float3 vColor);
	HRESULT Render_Font_Size(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition, const _float2& vSize, _float3 vColor);
#pragma endregion

#pragma region PICKING
	_bool Picking(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC);
#pragma endregion

private:
	class CGraphic_Device*		m_pGraphic_Device = { nullptr };
	class CLevel_Manager*		m_pLevel_Manager = { nullptr };
	class CPrototype_Manager*	m_pPrototype_Manager = { nullptr };
	class CObject_Manager*		m_pObject_Manager = { nullptr };
	class CRenderer*			m_pRenderer = { nullptr };
	class CTimer_Manager*		m_pTimer_Manager = { nullptr };
	class CSound_Manager*		m_pSound_Manager = { nullptr };
	class CPool_Manager*		m_pPool_Manager = { nullptr };
	class CCollider_Manager*	m_pCollider_Manager = { nullptr };
	class CFont_Manager*	    m_pFont_Manager = { nullptr };
	class CPartilce_Manager*	m_pParticle_Manager = { nullptr };
	/*class CPicking*				m_pPicking = { nullptr };*/

public:
	void Release_Engine();
	virtual void Free() override;
};

END
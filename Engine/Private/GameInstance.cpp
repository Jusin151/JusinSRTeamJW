#include "GameInstance.h"

#include "Picking.h"
#include "Renderer.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "Sound_Manager.h"
#include "Graphic_Device.h"
#include "PoolManager.h"
#include "Font_Manager.h"
#include "Object_Manager.h"
#include "Collider_Manager.h"
#include "Prototype_Manager.h"


IMPLEMENT_SINGLETON(CGameInstance);

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, LPDIRECT3DDEVICE9* ppOut)
{
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowed, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppOut);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pPrototype_Manager = CPrototype_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pPrototype_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppOut);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pSound_Manager = CSound_Manager::Create(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL);
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	m_pPool_Manager = CPool_Manager::Create();
	if (nullptr == m_pPool_Manager)
		return E_FAIL;

	m_pCollider_Manager = CCollider_Manager::Create();
	if (nullptr == m_pCollider_Manager)
		return E_FAIL;


	m_pFont_Manager = CFont_Manager::Create(*ppOut);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;
	/*m_pPicking = CPicking::Create(*ppOut, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pPicking)
		return E_FAIL;*/




	return S_OK;
}

void CGameInstance::Update_Engine(_float fTimeDelta)
{
	m_pObject_Manager->Priority_Update(fTimeDelta);

	m_pLevel_Manager->Update(fTimeDelta);
	m_pCollider_Manager->Update_Collison();

	/*m_pPicking->Update();*/

	m_pObject_Manager->Update(fTimeDelta);	

	m_pObject_Manager->Late_Update(fTimeDelta);
}

HRESULT CGameInstance::Draw()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->Render_Begin();

	m_pRenderer->Draw();

	m_pLevel_Manager->Render();

	m_pGraphic_Device->Render_End();

	return S_OK;
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	/* 특정 레벨의 자원을 삭제한다. */
	
	/* 특정 레벨의 객체을 삭제한다. */
	m_pObject_Manager->Clear(iLevelIndex);

	/* 특정 레벨의 원형객을 삭제한다. */
	m_pPrototype_Manager->Clear(iLevelIndex);
}

#pragma region LEVEL_MANAGER

HRESULT CGameInstance::Change_Level(_uint iLevelIndex, CLevel* pNewLevel)
{
	return m_pLevel_Manager->Change_Level(iLevelIndex, pNewLevel);
}
#pragma endregion

#pragma region PROTOTYPE_MANAGER

HRESULT CGameInstance::Add_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	return m_pPrototype_Manager->Add_Prototype(iPrototypeLevelIndex, strPrototypeTag, pPrototype);
}

CBase* CGameInstance::Clone_Prototype(PROTOTYPE ePrototypeType, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pPrototype_Manager->Clone_Prototype(ePrototypeType, iPrototypeLevelIndex, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Find_Prototype(const _wstring& strPrototypeTag)
{
	return m_pPrototype_Manager->Find_Prototype(strPrototypeTag);
}
#pragma endregion

#pragma region OBJECT_MANAGER
HRESULT CGameInstance::Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, void* pArg)
{
 	return m_pObject_Manager->Add_GameObject(iPrototypeLevelIndex, strPrototypeTag, iLevelIndex, strLayerTag, pArg);
}

CGameObject* CGameInstance::Add_GameObject_FromPool(_uint iPrototypeLevelIndex, _uint iLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	return m_pObject_Manager->Add_GameObject_FromPool(iPrototypeLevelIndex,iLevelIndex,strLayerTag, pArg);
}

CGameObject* CGameInstance::Find_Object(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Find_Object(iLevelIndex, strLayerTag);
}

CComponent* CGameInstance::Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag)
{
	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag,strComponentTag);
}

CGameObject* CGameInstance::Find_Last_Object(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Find_Last_Object(iLevelIndex, strLayerTag);
}


/*CComponent* CGameInstance::Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
	
}*/

#pragma endregion

#pragma region RENDERER

HRESULT CGameInstance::Add_RenderGroup(CRenderer::RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
	return m_pRenderer->Add_RenderGroup(eRenderGroup, pRenderObject);
}

#pragma endregion

#pragma region TIMER_MANAGER

_float CGameInstance::Get_TimeDelta(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Get_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

void CGameInstance::Update_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Update(strTimerTag);
}

#pragma endregion
#pragma region PICKING
_bool CGameInstance::Picking(_float3& vPickedPos, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC)
{
	return false;
	//return m_pPicking->Picking(vPickedPos, vPointA, vPointB, vPointC);
}

#pragma endregion

#pragma region POOL_MANAGER

CGameObject* CGameInstance::Acquire_Object(_uint iPrototypeLevelIndex, const _wstring& strLayerTag,void* pArg)
{
	return m_pPool_Manager->Acquire_Object(iPrototypeLevelIndex,strLayerTag,pArg);
}

HRESULT CGameInstance::Return_Object(_uint iPrototypeLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject)
{
	return  m_pPool_Manager->Return_Object(iPrototypeLevelIndex,strLayerTag,pGameObject);
}

HRESULT CGameInstance::Reserve_Pool(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strLayerTag, _uint iCount, void* pArg)
{
	return  m_pPool_Manager->Reserve_Pool(iPrototypeLevelIndex, strPrototypeTag, strLayerTag, iCount, pArg);
}

#pragma endregion

#pragma region COLLIDER_MANAGER

HRESULT CGameInstance::Add_Collider(COLLIDERGROUP eGroup, CCollider* Collider)
{
	return m_pCollider_Manager->Add_Collider(eGroup, Collider);
}

vector<list<class CCollider*>> CGameInstance::Get_Colliders()
{
	return m_pCollider_Manager->Get_Colliders();
}

HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _wstring& strFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition, _float3 vColor)
{
	return m_pFont_Manager->Render_Font(strFontTag,strText,vPosition,vColor);
}

// 사이즈 크기 조절하려고 홍동완이 만듬
HRESULT CGameInstance::Render_Font_Size(const _wstring& strFontTag, const _wstring& strText, const _float2& vPosition,const _float2& vSize, _float3 vColor)
{
	return m_pFont_Manager->Render_Font_size(strFontTag, strText, vPosition, vSize, vColor);
}

#pragma endregion

#pragma region Graphic_Device

void CGameInstance::Change_ClearColor(_float4 rgba)
{
	return m_pGraphic_Device->ChangeClearColor(rgba);
}

#pragma endregion

void CGameInstance::Release_Engine()
{

	Safe_Release(m_pFont_Manager);
	/*Safe_Release(m_pPicking);*/

	Safe_Release(m_pTimer_Manager);

	Safe_Release(m_pRenderer);

	Safe_Release(m_pSound_Manager);

	Safe_Release(m_pObject_Manager);
	
	Safe_Release(m_pPool_Manager);

	Safe_Release(m_pPrototype_Manager);

	Safe_Release(m_pLevel_Manager);

	Safe_Release(m_pCollider_Manager);

	Safe_Release(m_pGraphic_Device);


	Destroy_Instance();
}

void CGameInstance::Free()
{
	__super::Free();



	
}

#pragma once

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device;
class CGameObject;

class CMyImGui final : public CBase
{
private:
	CMyImGui(CGraphic_Device* pGraphic_Device);
	virtual ~CMyImGui() = default;
public:
	HRESULT Initialize(_uint iNumLevels, HWND hWnd, CGraphic_Device* pGraphic_Device);
	void Update(_float fTimeDelta);
	HRESULT Render();
	HRESULT RegisterGameObject(CGameObject*);
	HRESULT RegisterMainCamera(CGameObject*);

private:
	void ShowLayerInMap(map<const _wstring, class CLayer*>* pLayers);
	void ShowListInLayer(CLayer* pLayer);
	void ShowComponentsInGameObject(CGameObject* pGameObject);

private:
	bool show_demo_window = true;
	bool show_another_window = true;
	_float4 clear_color = { 0.45f, 0.55f, 0.60f, 1.00f };

private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	class CGraphic_Device*	m_pGraphic_Device = { nullptr };
	_uint m_iNumLevels = {};
	std::map<unsigned int, CGameObject*> m_gameObjects;
	
public:
	static CMyImGui* Create(_uint iNumLevels, HWND hWnd, CGraphic_Device* pGraphic_Device);
	void Free();
};

END
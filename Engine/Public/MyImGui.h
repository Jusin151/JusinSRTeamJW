#pragma once

#include "Base.h"
#include "../../Client/public/Structure.h"
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

	void Show_Texture_Image();
	void Show_Objects(_float fTimeDelta);
	void LoadImagesFromFolder(const _wstring& folderPath);
	_wstring SelectFile();
	_wstring SelectFolder();
	HRESULT CreateObject();
	_wstring GetRelativePath(const _wstring& absolutePath);


private:

	bool show_demo_window = true;
	bool show_another_window = true;
	_float4 clear_color = { 0.45f, 0.55f, 0.60f, 1.00f };


#pragma region 텍스쳐용 멤버 변수
	vector<LPDIRECT3DTEXTURE9> m_Textures;
	_ulonglong m_SelectedTextureIndex = -1;
	_bool m_bShowImageWindow = true;
#pragma endregion

#pragma region 오브젝트용 멤버 변수
	// 테스트용/////////////////////////////
	_bool m_bRenderSelectedCube = false; // 선택된 큐브를 렌더링할지 여부
	_bool m_bShowObjectsWindow = false; 
	_ulonglong m_iCurrentObject{0};

	vector<class CVIBuffer_Cube*> m_pVIBuffer_CubeVec;
	vector<class CTransform*> m_pTransformVec;
	vector<class CCollider*> m_pColliderVec;
	_wstring m_wstrProjectPath; // 프로젝트 루트 경로 저장

	///////////////////////////////////////
#pragma endregion

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
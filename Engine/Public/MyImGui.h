#pragma once

#include "Base.h"

BEGIN(Engine)

class CMyImGui final : public CBase
{
private:
	CMyImGui(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CMyImGui() = default;
public:
	HRESULT Initialize(HWND hWnd, LPDIRECT3DDEVICE9 pGraphic_Device);
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	void Show_Texture_Image();
	void Show_Objects();
	void LoadImagesFromFolder(const _wstring& folderPath);
	_wstring SelectFolder();
	HRESULT CreateObject();

private:
	LPDIRECT3DDEVICE9		m_pGraphic_Device = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };
	bool show_demo_window = true;
	bool show_another_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

#pragma region 텍스쳐용 멤버 변수
	vector<LPDIRECT3DTEXTURE9> m_Textures;
	_ulonglong m_SelectedTextureIndex = -1;
	_bool m_bShowImageWindow = true;
#pragma endregion

#pragma region 오브젝트용 멤버 변수
	// 테스트용/////////////////////////////
	_bool m_bRenderSelectedCube = false; // 선택된 큐브를 렌더링할지 여부
	_bool m_bShowObjectsWindow = false; 
	_uint m_iCurrentObject{0};

	vector<class CVIBuffer_Cube*> m_pVIBuffer_Cube;
	vector<class CTransform*>m_pTransform;
	///////////////////////////////////////
#pragma endregion

public:
	static CMyImGui* Create(HWND hWnd, LPDIRECT3DDEVICE9 pGraphic_Device);
	void Free();
};

END
#pragma once
#include "Base.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CEditor;
class CGameObject;
class CGameInstance;
class CTransform;
END

BEGIN(Client)

class CMyImGui  final : public CBase
{
	struct PrototypeInfo
	{
		string tag;             // 게임 오브젝트 태그
		string className;       // 클래스 이름
		int level;              // 레벨
		string textureTag;      // 관련 텍스처 태그 (매칭할 예정)
		string texturePath;     // 텍스처 경로
		int textureCount;       // 텍스처 개수
		string bufferTag;       // 관련 버퍼 태그
	};
private:
	CMyImGui(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CMyImGui() = default;
public:
	HRESULT Initialize(_uint iNumLevels, LPDIRECT3DDEVICE9 pGraphic_Device);
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
//	void ShowLayerInMap(map<const _wstring, class CLayer*>* pLayers);
	//void ShowListInLayer(CLayer* pLayer);
	void ShowComponentsInGameObject(CGameObject* pGameObject);

private:
#pragma region 에디터용
	void Show_Objects();
	void LoadImagesFromFolder(const _wstring& folderPath);
	_wstring SelectFile();
	_wstring SelectFolder();
	//HRESULT CreateObject();
	_wstring GetRelativePath(const _wstring& absolutePath);

	void ShowInspectorTab();
	void ShowCreateObjectTab();
	HRESULT CreateObjectInstance(
		const char* jsonFileNameBuffer,
		const char* bufferNameBuffer, const char* colliderNameBuffer,
		const char* layerTagBuffer, const char* textureTagBuffer,
		const char* objectClassTagBuffer, const char* objectProtoTagBuffer,
		const char* objectLayerTagBuffer, _int iPoolingCount,
		_int iLevel, _int iProtoLevel, const _wstring& selectedFolder);
	HRESULT SaveObjectToJson(
		const string& jsonFileName,
		const _wstring& objectProtoTag,
		_int objectLevel,
		const _wstring& className,
		const _wstring& textureTag,
		_int textureLevel,
		const _wstring& texturePath,
		_int textureCount,
		const _wstring& bufferTag,
		_int bufferLevel,
		const _wstring& bufferClass,
		_int bufferWidth,
		_int bufferHeight);
	void Remove_Object();
	HRESULT LoadPrototypesFromJson(const string& jsonFileName, vector<PrototypeInfo>& outPrototypes);
	// ImGuizmo 관련 함수 추가
	void ConfigureImGuizmo();
	void RenderImGuizmo(CTransform* pTransform);
	void InputKey();

	void LoadSingleImageFile(const _wstring& filePath);
	void LoadImagesFromDirectoryPath(const _wstring& folderPath);
	void ShowPrevTextureImage();


#pragma endregion
public:
	void Set_Object(CGameObject* pGameObject = nullptr) { m_pCurrentGameObject = pGameObject; }
	static _bool IsMouseOverImGui();
private:

	bool show_demo_window = true;
	bool show_another_window = true;
	_float4 clear_color = { 0.45f, 0.55f, 0.60f, 1.00f };


	// ImGuizmo 관련 변수 추가
	ImGuizmo::OPERATION m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE m_CurrentGizmoMode = ImGuizmo::LOCAL;
	bool m_bUseSnap = false;
	float m_SnapValue[3] = { 1.0f, 1.0f, 1.0f };

#pragma region 텍스쳐용 멤버 변수
	vector<LPDIRECT3DTEXTURE9> m_Textures;
	_bool m_bShowImageWindow = true;
	_ulonglong m_SelectedTextureIndex = 0;
	// 텍스처 파일 경로 저장
	vector<_wstring> m_TextureFilePaths;
	_wstring m_wstrSelectedTexturePath;   // 선택된 텍스처 경로
	char m_szSelectedPathBuffer[512];     // 경로 표시용 버퍼
#pragma endregion

#pragma region 오브젝트용 멤버 변수
	// 테스트용/////////////////////////////
	_bool m_bRenderSelectedCube = false; // 선택된 큐브를 렌더링할지 여부
	_bool m_bShowObjectsWindow = false; 
	_ulonglong m_iCurrentObject{0};

	//vector<class CVIBuffer_Cube*> m_pVIBuffer_CubeVec;
	//vector<class CTransform*> m_pTransformVec;
	//vector<class CCollider*> m_pColliderVec;
	_wstring m_wstrProjectPath; // 프로젝트 루트 경로 저장

	///////////////////////////////////////
#pragma endregion

private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	LPDIRECT3DDEVICE9	m_pGraphic_Device = { nullptr };
	_uint m_iNumLevels = {};
	map<unsigned int, CGameObject*> m_gameObjects;
	CEditor* m_Editor = { nullptr };
	CGameObject* m_pCurrentGameObject = { nullptr };
public:
	static CMyImGui* Create(_uint iNumLevels,LPDIRECT3DDEVICE9 pGraphic_Device);
	void Free();
};

END
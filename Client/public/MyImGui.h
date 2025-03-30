#pragma once
#include "Base.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CEditor;
class CGameInstance;
class CGameObject;
class CTransform;
END

BEGIN(Client)

class CMyImGui  final : public CBase
{
public:
	enum class EHistoryActionType
	{
		NONE,
		TRANSFORM_POSITION,
		TRANSFORM_ROTATION,
		TRANSFORM_SCALE,
		OBJECT_CREATE,
		OBJECT_DELETE
	};

	struct tHistoryItem
	{


		CMyImGui::EHistoryActionType eType = CMyImGui::EHistoryActionType::NONE;
		CGameObject* pGameObject = nullptr;
		_wstring wstrLayerTag;
		_uint iLevel = 0;
		_uint iProtoLevel = 0;



		// 트랜스폼 관련 상태 저장
		_float3 vOldPosition = { 0.0f, 0.0f, 0.0f };
		_float3 vNewPosition = { 0.0f, 0.0f, 0.0f };
		_float3 vOldRotation = { 0.0f, 0.0f, 0.0f };
		_float3 vNewRotation = { 0.0f, 0.0f, 0.0f };
		_float3 vOldScale = { 0.0f, 0.0f, 0.0f };
		_float3 vNewScale = { 0.0f, 0.0f, 0.0f };

		// 생성/삭제 관련 정보
		void* tObjDesc = { nullptr };
		_wstring wstrPrototypeTag;
		_uint objectID = -1;
	};
	
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
	void ShowItemCreationTab();// 아이템 제작
	void ShowInspectorTab(); // 인스펙터
	void ShowCreateObjectTab(); // 오브젝트 생성
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
		_int bufferWidth = 256,
		_int bufferHeight = 256);
	void Remove_Object();
	void Duplicate_Object();
	CGameObject* Find_Object_ByAddress(CGameObject* pAddress);
	HRESULT LoadPrototypesFromJson(const string& jsonFileName, vector<PrototypeInfo>& outPrototypes);

	void RenderImGuizmo(CTransform* pTransform);
	void InputKey();

	void LoadSingleImageFile(const _wstring& filePath);
	void LoadImagesFromDirectoryPath(const _wstring& folderPath);
	void ShowPrevTextureImage();


	// 히스토리 스택
	stack<tHistoryItem> m_vecUndoStack;

	// 트랜스폼 이전 상태 추적용 변수
	_float3 m_vPrevPosition = { 0.0f, 0.0f, 0.0f };
	_float3 m_vPrevRotation = { 0.0f, 0.0f, 0.0f };
	_float3 m_vPrevScale = { 0.0f, 0.0f, 0.0f };
	_bool m_bTrackingTransform = false;
	_bool m_bCreateItem = { false };

	_float m_fLastInputTime = 0.0f;
	const _float m_fInputCooldown = 0.01f; // 300ms 쿨다운

	// 히스토리 관련 함수
	void AddToHistory(const tHistoryItem& item);
	void BeginTransformAction();
	void EndTransformAction();
	void Undo();

	// ID 관련 함수
	_uint AssignObjectID(CGameObject* pObj);
	void AssignSpecificObjectID(CGameObject* pObj, _uint specificID); // 삭제할 때 기존 아이디 넘겨주기
	CGameObject* GetObjectByID(UINT id);
	void RemoveObjectID(CGameObject* pObj);


	static _uint m_NextObjectID;
	map<CGameObject*, UINT> m_ObjectIDMap;
	map<UINT, CGameObject*> m_IDObjectMap;



#pragma endregion
public:
	void Set_Object(CGameObject* pGameObject = nullptr) {
	
		m_pCurrentGameObject = pGameObject;
	}
	static _bool IsMouseOverImGui();
private:

	bool show_demo_window = true;
	bool show_another_window = true;
	_float4 clear_color = { 0.45f, 0.55f, 0.60f, 1.00f };


	// ImGuizmo 관련 변수 추가
	ImGuizmo::OPERATION m_CurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE m_CurrentGizmoMode = ImGuizmo::WORLD;
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
	tHistoryItem m_tHistoryItem; // 복제용 히스토리 아이템

	///////////////////////////////////////
#pragma endregion
#pragma region 트리거용 멤버 변수
public:
		void ShowTriggerTab();
		HRESULT CreateTriggerInstance(const _wstring& strTargetTag, _uint iTriggerType,
			const _float3& vPosition, _float fActivationRange, _bool bStartsActive, const _wstring& textureTag);
private:
	_bool m_bCreateTrigger = false;
	CGameObject* m_pSelectedTarget = nullptr;
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
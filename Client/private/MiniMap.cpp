#include "MiniMap.h"
#include "Player.h"
#include <Transform.h>
#include <StructureManager.h>
#include <Camera_FirstPerson.h>

constexpr _float VIEW_RANGE = 25.f;
constexpr _uint MINIMAP_SIZE = 256;
CMiniMap::CMiniMap(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device),
	m_MiniMapRect(),
	m_pMiniMapSurface(nullptr),
	m_pMiniMapTexture(nullptr),
	m_pSprite(nullptr),
	m_pVertexBuffer(nullptr),
	m_bStaticMapRendered(false)
{
}

CMiniMap::CMiniMap(const CMiniMap& Prototype)
	: CGameObject(Prototype),
	m_pMiniMapSurface(Prototype.m_pMiniMapSurface),
	m_pMiniMapTexture(Prototype.m_pMiniMapTexture),
	m_pVertexBuffer(Prototype.m_pVertexBuffer),
	m_pSprite(Prototype.m_pSprite),
	m_MiniMapRect(Prototype.m_MiniMapRect),
	m_bStaticMapRendered(false),
	m_bIsOriginal(false)
{
	Safe_AddRef(m_pMiniMapSurface);
	Safe_AddRef(m_pMiniMapTexture);
	Safe_AddRef(m_pVertexBuffer);
	Safe_AddRef(m_pSprite);
}

HRESULT CMiniMap::Initialize_Prototype()
{
	m_pGraphic_Device->CreateTexture(MINIMAP_SIZE, MINIMAP_SIZE, 1, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
		&m_pMiniMapTexture, NULL);
	m_pMiniMapTexture->GetSurfaceLevel(0, &m_pMiniMapSurface);

	// 스프라이트 객체 생성
	D3DXCreateSprite(m_pGraphic_Device, &m_pSprite);

	// 미니맵 출보 위치 및 크기 설정
	m_MiniMapRect = { 10, 10, 10 + 150, 10 + 150 }; // 좌상단 위치(10,10)에 150x150 크기로 표시

	// 정점 버퍼 생성
	m_pGraphic_Device->CreateVertexBuffer(
		4 * sizeof(VERTEX), // 정점 4개
		0,
		VERTEX::FVF,
		D3DPOOL_MANAGED,
		&m_pVertexBuffer,
		NULL
	);

	return S_OK;
}

HRESULT CMiniMap::Initialize(void* pArg)
{
	if(FAILED(Ready_Components()))
		return E_FAIL;
	// 구조물 리스트 가져오기
	m_DoorList = CStructureManager::Get_Instance()->Get_Doors();
	m_StructureList = CStructureManager::Get_Instance()->Get_Structures();

	// 플레이어 찾기
	m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_Player"));
	if (!m_pPlayer)
	{
		m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, L"Layer_Player"));
	}

	if (!m_pPlayer)
	{
		return E_FAIL;
	}

	//m_pTransformCom->Set_Scale(-1.f, -1.f, 1.f);

	// 3. Transform 행렬 바인딩
	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,_float3(200.f,200.f, 0.0f));
	D3DXMatrixOrthoLH(&m_matMiniMapProj, VIEW_RANGE, VIEW_RANGE, 0.1f, 1000.0f);

	CCamera_FirstPerson* pCamera = dynamic_cast<CCamera_FirstPerson*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, L"Layer_Camera"));
	if (pCamera)
	{
		m_pCamera = pCamera;
	}


	return S_OK;
}
void CMiniMap::Update(_float fTimeDelta)
{
	
}

void CMiniMap::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CMiniMap::Render()
{

	// 1. 원래 렌더 타겟 저장
	LPDIRECT3DSURFACE9 pBackBuffer = nullptr;
	m_pGraphic_Device->GetRenderTarget(0, &pBackBuffer);

	// 2. 렌더 타겟을 미니맵 텍스처로 변경
	m_pGraphic_Device->SetRenderTarget(0, m_pMiniMapSurface);

	// 3. 미니맵 텍스처 클리어 (배경색 회색)
	m_pGraphic_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(50, 50, 50), 1.0f, 0);
	
	SetUp_RenderState();
	for (const auto& structure : m_StructureList)
	{
		if (!structure) continue;
		if (structure->Get_Tag().find(L"Floor") != std::wstring::npos ||
			structure->Get_Tag().find(L"Ceil") != std::wstring::npos ||
			structure->Get_Tag().find(L"Magma") != std::wstring::npos)
			continue;

		structure->Render();
	}

	for (const auto& door : m_DoorList)
	{
		if (door && !door->IsOpen())
		{
			door->Render();
		}
	}


	if (m_pPlayer)
	{
		RenderPlayerOnMiniMap();
	}

	Release_RenderState();
	m_pGraphic_Device->SetRenderTarget(0, pBackBuffer);
	Safe_Release(pBackBuffer);

	if (FAILED(m_pTransformCom->Bind_Resource()))
		return E_FAIL;
	// 6. 미니맵 텍스처를 화면에 그리기
	if (m_pSprite && m_pMiniMapTexture)
	{
		// 스프라이트 시작
		m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

		if (m_pTransformCom)
		{
			_float4x4 matWorld = m_pTransformCom->Get_WorldMat();
			m_pSprite->SetTransform(&matWorld);
		}

		D3DXVECTOR3 center(MINIMAP_SIZE / 2.f, MINIMAP_SIZE / 2.f, 0.f);
		m_pSprite->Draw(m_pMiniMapTexture, NULL, &center, NULL, D3DCOLOR_ARGB(200, 255, 255, 255));
	//	m_pSprite->Draw(m_pMiniMapTexture, NULL, NULL, NULL, D3DCOLOR_ARGB(180, 255, 255, 255)); // 약간 투명하게

		m_pSprite->End();
	}

	return S_OK;
}

HRESULT CMiniMap::SetUp_RenderState()
{
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &m_OldProjMatrix);
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &m_OldViewMatrix);

	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, false);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, true);

	CTransform* pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
	CTransform* pCameraTransform = static_cast<CTransform*>(m_pCamera->Get_Component(TEXT("Com_Transform")));
	if (pPlayerTransform)
	{

	_float3 vPos = pCameraTransform->Get_State(CTransform::STATE_POSITION);
	_float3 vLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
	_float3 vLook2 = pCameraTransform->Get_State(CTransform::STATE_LOOK);
	_float fAngleY = m_pCamera->Get_Yaw();
	D3DXVECTOR3 eye(vPos.x, vPos.y + 10.f, vPos.z);
	D3DXVECTOR3 at(vPos.x , vPos.y, vPos.z );
	D3DXVECTOR3 up(0.0f, 0.0f, 1.0f);
	m_pTransformCom->Rotate_EulerAngles(_float3(0.f, 0.f,fAngleY));
	D3DXMatrixLookAtLH(&m_matMiniMapView, &eye, &at, &up);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_matMiniMapView);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_matMiniMapProj);

	}
	return S_OK;
}

HRESULT CMiniMap::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, true);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, false); // 조명 상태 복원
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_OldProjMatrix);
	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_OldViewMatrix); // 뷰 행렬 복원
	return S_OK;
}

HRESULT CMiniMap::Ready_Components()
{
	CTransform::TRANSFORM_DESC		TransformDesc{ 20.f, D3DXToRadian(180.f) };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom),&TransformDesc)))
		return E_FAIL;
	return S_OK;
}

void CMiniMap::RenderPlayerOnMiniMap()
{
	if (!m_pPlayer)
		return;
	// 플레이어 Transform 얻기
	CTransform* pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
	if (!pPlayerTransform)
		return;

	// 월드 좌표 위치 및 Yaw 가져오기
	_float3 vPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_float fYaw = m_pCamera->Get_Yaw();  // 카메라의 회전값을 사용할 수도 있음

	// 회전 반영할 월드 행렬 구성
	D3DXMATRIX matTrans, matRotZ, matWorld;
	D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y+0.5f, vPos.z);
	D3DXMatrixRotationY(&matRotZ, fYaw);
	matWorld = matRotZ*matTrans;
	auto worldMat = matRotZ*pPlayerTransform->Get_WorldMat();
	// 월드 행렬 설정
	m_pGraphic_Device->SetTransform(D3DTS_WORLD, &worldMat);

	static _bool bInit = { false };

	if (!bInit)
	{

	// 삼각형 정점 생성
	float size =1.5f; // 삼각형 크기
	VERTEX vertices[3] = {
		{ D3DXVECTOR3(0.f, 0.f, size), D3DCOLOR_RGBA(255, 0, 0, 255) },
		{ D3DXVECTOR3(size * 0.7f, 0.f, -size * 0.7f), D3DCOLOR_RGBA(255, 0, 0, 255) },
		{ D3DXVECTOR3(-size * 0.7f, 0.f, -size * 0.7f), D3DCOLOR_RGBA(255, 0, 0, 255) }
	};

	// 정점 버퍼에 쓰기
	void* pVertices = nullptr;
	m_pVertexBuffer->Lock(0, sizeof(vertices), &pVertices, 0);
	memcpy(pVertices, vertices, sizeof(vertices));
	m_pVertexBuffer->Unlock();
	bInit = true;
	}

	m_pGraphic_Device->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(VERTEX));
	m_pGraphic_Device->SetFVF(VERTEX::FVF);
	m_pGraphic_Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
}


json CMiniMap::Serialize()
{
	return json();
}

void CMiniMap::Deserialize(const json& j)
{
}

CMiniMap* CMiniMap::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMiniMap* pInstance = new CMiniMap(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMiniMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMiniMap::Clone(void* pArg)
{
	CMiniMap* pInstance = new CMiniMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMiniMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniMap::Free()
{
	if (m_bIsOriginal)
	{
		_uint refCount = 0;
		do {
			refCount = Safe_Release(m_pMiniMapSurface);
		} while (refCount > 0);
	}
	else
	{
		Safe_Release(m_pMiniMapSurface);
	}

	Safe_Release(m_pMiniMapTexture);
	Safe_Release(m_pVertexBuffer);
	Safe_Release(m_pSprite); // 스프라이트 해제 추가
	Safe_Release(m_pTransformCom);
	__super::Free();
	//Safe_Release(m_pTransformCom);
	//Safe_Release(m_pMaterialCom);
}
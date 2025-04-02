#include "MiniMap.h"
#include "Player.h"
#include <Transform.h>
#include <StructureManager.h>

const int GRID_SIZE = 32;      // 그리드 하나의 크기 (32x32 그리드)
const int MINIMAP_SIZE = 256;  // 미니맵 텍스처 크기

// 그리드 셀 타입 정의
enum GRID_CELL_TYPE
{
    GRID_CELL_CORRIDOR = 0, // 복도
    GRID_CELL_WALL = 1,     // 벽
    GRID_CELL_DOOR = 2      // 문
};


CMiniMap::CMiniMap(LPDIRECT3DDEVICE9 pGraphic_Device)
    :CGameObject(pGraphic_Device),
    m_MiniMapRect()
{
}

CMiniMap::CMiniMap(const CMiniMap& Prototype)
    : CGameObject(Prototype),
    m_pMiniMapSurface(Prototype.m_pMiniMapSurface),
    m_pMiniMapTexture(Prototype.m_pMiniMapTexture),
    m_MiniMapRect(Prototype.m_MiniMapRect)
{
  //  Safe_AddRef(m_pMiniMapSurface);
  //  Safe_AddRef(m_pMiniMapTexture);
}

HRESULT CMiniMap::Initialize_Prototype()
{
  
    return S_OK;
}

HRESULT CMiniMap::Initialize(void* pArg)
{ // 미니맵 텍스처 생성
    m_pGraphic_Device->CreateTexture(MINIMAP_SIZE, MINIMAP_SIZE, 1, D3DUSAGE_RENDERTARGET,
        D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
        &m_pMiniMapTexture, NULL);
    m_pMiniMapTexture->GetSurfaceLevel(0, &m_pMiniMapSurface);

    // 스프라이트 객체 생성
    D3DXCreateSprite(m_pGraphic_Device, &m_pSprite);

    // 미니맵 출력 위치 및 크기 설정
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

    // 맵 사이즈 계산 및 그리드 맵 초기화
    CalculateMapSize();
    InitializeGridMap();
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

    if (!m_bStaticMapRendered)
    {
     
    }

    // 1. 원래 렌더 타겟 저장
    LPDIRECT3DSURFACE9 pBackBuffer = nullptr;
    m_pGraphic_Device->GetRenderTarget(0, &pBackBuffer);

    // 2. 렌더 타겟을 미니맵 텍스처로 변경
    m_pGraphic_Device->SetRenderTarget(0, m_pMiniMapSurface);

    // 3. 미니맵 텍스처 클리어 (배경색을 어두운 회색으로 설정 - 복도 표현)
    m_pGraphic_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(50, 50, 50), 1.0f, 0);

    SetUp_RenderState();
    for (const auto& structure : m_StructureList)
    {
        // "Floor"나 "Ceil" 태그가 있는 것은 건너뜁니다.
        if (structure->Get_Tag().find(L"Floor") != std::wstring::npos ||
            structure->Get_Tag().find(L"Ceil") != std::wstring::npos ||
            structure->Get_Tag().find(L"Magma") != std::wstring::npos)
            continue;

        structure->Render();
    }
    if (m_pPlayer)
    {
        RenderPlayerOnMiniMap();
    }
    m_bStaticMapRendered = true;
    //m_pPlayer->Render();
    Release_RenderState();
    // 5. 원래 렌더 타겟으로 복원
    m_pGraphic_Device->SetRenderTarget(0, pBackBuffer);
    Safe_Release(pBackBuffer);
    //RenderTestSquare();


    // 6. 미니맵 텍스처를 화면에 그리기
    if (m_pSprite && m_pMiniMapTexture)
    {
        // 스프라이트 시작
        m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

        // 미니맵 텍스처를 화면 좌상단에 그리기
        D3DXVECTOR3 pos(100.0f, 100.0f, 0.0f); // 위치 조정

        // 텍스처 크기 조정
        D3DXMATRIX matScale;
        D3DXMatrixScaling(&matScale, 1.f, 1.f, 1.0f);

        D3DXMATRIX matTranslation;
        D3DXMatrixTranslation(&matTranslation, pos.x, pos.y, 0.0f);

        D3DXMATRIX matFinal = matScale * matTranslation;
        m_pSprite->SetTransform(&matFinal);

        m_pSprite->Draw(m_pMiniMapTexture, NULL, NULL, NULL, D3DCOLOR_ARGB(180, 255, 255, 255)); // 약간 투명하게

        // 스프라이트 종료
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

    // 👇 플레이어 위에서 아래로 보는 뷰 행렬
    _float3 vPos = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
    D3DXVECTOR3 eye(vPos.x, vPos.y + 5.f, vPos.z );  // 위에서
    D3DXVECTOR3 at(vPos.x, vPos.y, vPos.z);           // 플레이어 중심
    D3DXVECTOR3 up(0.0f, 0.0f, 1.0f);                 // y축을 위로

    D3DXMATRIX matView;
    D3DXMatrixLookAtLH(&matView, &eye, &at, &up);
    m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matView);

    // 👇 플레이어 주변 일정 범위만 보이게 직교 투영 설정
    const float VIEW_RANGE = 20.f; // 플레이어 주변 50 유닛
    D3DXMATRIX matOrtho;
    D3DXMatrixOrthoLH(&matOrtho, VIEW_RANGE, VIEW_RANGE, 0.1f, 1000.0f);
    m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOrtho);

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
    return E_NOTIMPL;
}

void CMiniMap::CalculateMapSize()
{
    //float minX = FLT_MAX, minZ = FLT_MAX;
    //float maxX = FLT_MIN, maxZ = FLT_MIN;

    //// 벽과 문의 위치를 전부 확인
    //for (const auto& structure : m_StructureList)
    //{
    //    _float3 pos = dynamic_cast<CTransform*>(structure->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
    //    minX = min(minX, pos.x);
    //    maxX = max(maxX, pos.x);
    //    minZ = min(minZ, pos.z);
    //    maxZ = max(maxZ, pos.z);
    //}

    //for (const auto& door : m_DoorList)
    //{
    //    _float3 pos = dynamic_cast<CTransform*>(door->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
    //    minX = min(minX, pos.x);
    //    maxX = max(maxX, pos.x);
    //    minZ = min(minZ, pos.z);
    //    maxZ = max(maxZ, pos.z);
    //}

    //m_MapWidth = maxX - minX;
    //m_MapHeight = maxZ - minZ;

    //// 최소 좌표 저장 (좌표 변환에 사용)
    //m_MapMinX = minX;
    //m_MapMinZ = minZ;

    //// 그리드 크기 계산
    //m_GridColumns = _int(ceil(m_MapWidth / m_GridCellSize));
    //m_GridRows = _int(ceil(m_MapHeight / m_GridCellSize));

    float minX = FLT_MAX, minZ = FLT_MAX;
    float maxX = -FLT_MAX, maxZ = -FLT_MAX;

    // 구조물(벽, 바닥, 문 등) 좌표 고려
    for (const auto& structure : m_StructureList)
    {
        if (structure->Get_Tag().find(L"Floor") != std::wstring::npos ||
            structure->Get_Tag().find(L"Ceil") != std::wstring::npos)
            continue;
        _float3 pos = dynamic_cast<CTransform*>(structure->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
        minX = min(minX, pos.x);
        maxX = max(maxX, pos.x);
        minZ = min(minZ, pos.z);
        maxZ = max(maxZ, pos.z);
    }

    for (const auto& door : m_DoorList)
    {
        _float3 pos = dynamic_cast<CTransform*>(door->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
        minX = min(minX, pos.x);
        maxX = max(maxX, pos.x);
        minZ = min(minZ, pos.z);
        maxZ = max(maxZ, pos.z);
    }

    // 플레이어 좌표도 포함시킵니다.
    if (m_pPlayer)
    {
        _float3 pPos = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
        minX = min(minX, pPos.x);
        maxX = max(maxX, pPos.x);
        minZ = min(minZ, pPos.z);
        maxZ = max(maxZ, pPos.z);
    }

    m_MapWidth = maxX - minX;
    m_MapHeight = maxZ - minZ;

    m_MapMinX = minX;
    m_MapMinZ = minZ;
}

_float2 CMiniMap::ConvertToMiniMapPos(_float3 vPos)
{
    //// 월드 좌표 -> 미니맵 좌표 변환
    //_float scaleX = 256.f / m_MapWidth; // 미니맵의 너비를 월드의 너비로 나눔
    //_float scaleY = 256.f / m_MapHeight; // 미니맵의 높이를 월드의 높이로 나눔

    //_float miniMapPosX = (vPos.x - m_MapMinX) * scaleX; // 월드 좌표를 미니맵 좌표로 변환
    //_float miniMapPosY = (vPos.z - m_MapMinZ) * scaleY; // 월드 좌표를 미니맵 좌표로 변환

    //return _float2(miniMapPosX, miniMapPosY);

      // 전체 월드 범위에 따른 스케일 계산 (미니맵 텍스처 크기는 256×256)
    float scaleX = 256.f / m_MapWidth;
    float scaleY = 256.f / m_MapHeight;

    // 플레이어의 월드 좌표를 가져옴 (CTransform 컴포넌트 이용)
    _float3 playerPos = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

    // 플레이어의 미니맵 상 좌표(현재 변환식 기준)
    float playerMiniMapX = (playerPos.x - m_MapMinX) * scaleX;
    float playerMiniMapY = (playerPos.z - m_MapMinZ) * scaleY;

    // 플레이어가 미니맵 중앙(128,128)에 오도록 하는 오프셋 계산
    float offsetX = (256.f / 2.0f) - playerMiniMapX;
    float offsetY = (256.f / 2.0f) - playerMiniMapY;

    // 입력된 월드 좌표를 미니맵 좌표로 변환하고 오프셋 적용
    float miniMapPosX = (vPos.x - m_MapMinX) * scaleX + offsetX;
    float miniMapPosY = (vPos.z - m_MapMinZ) * scaleY + offsetY;

    return _float2(miniMapPosX, miniMapPosY);
}

void CMiniMap::DrawBoxOnMiniMap(D3DXVECTOR2 pos, D3DCOLOR color, float size)
{
    VERTEX* pVertices;
    m_pGraphic_Device->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(VERTEX));
    m_pGraphic_Device->SetFVF(VERTEX::FVF);
    m_pVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);

    pVertices[0] = { D3DXVECTOR3(pos.x - size, pos.y - size, 0), color };
    pVertices[1] = { D3DXVECTOR3(pos.x + size, pos.y - size, 0), color };
    pVertices[2] = { D3DXVECTOR3(pos.x + size, pos.y + size, 0), color };
    pVertices[3] = { D3DXVECTOR3(pos.x - size, pos.y + size, 0), color };

    m_pVertexBuffer->Unlock();
    m_pGraphic_Device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
}

void CMiniMap::InitializeGridMap()
{ // 그리드 셀 크기 설정 (실제 월드 단위)
    m_GridCellSize = 1.0f; // 실제 월드 단위 셀 크기 설정

    // 그리드 행과 열 수 계산
    m_GridColumns = ceil(m_MapWidth / m_GridCellSize);
    m_GridRows = ceil(m_MapHeight / m_GridCellSize);

    // 그리드 맵 메모리 할당
    m_GridMap.resize(m_GridRows);
    for (int i = 0; i < m_GridRows; ++i)
    {
        m_GridMap[i].resize(m_GridColumns, GRID_CELL_CORRIDOR); // 기본값은 복도(이동 가능 영역)
    }

    // 구조물(벽, 문 등) 배치: 바닥과 천장은 제외
    for (const auto& structure : m_StructureList)
    {
        // "Floor"나 "Ceil" 태그가 있는 것은 건너뜁니다.
        if (structure->Get_Tag().find(L"Floor") != std::wstring::npos ||
            structure->Get_Tag().find(L"Ceil") != std::wstring::npos)
            continue;

        _float3 pos = dynamic_cast<CTransform*>(structure->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
        GridCoord gridPos = WorldToGrid(pos);

        if (IsValidGridCoord(gridPos))
        {
           
        }

        m_GridMap[gridPos.row][gridPos.col] = GRID_CELL_WALL;
    }

    for (const auto& door : m_DoorList)
    {
        _float3 pos = dynamic_cast<CTransform*>(door->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
        GridCoord gridPos = WorldToGrid(pos);

        if (IsValidGridCoord(gridPos))
        {
            m_GridMap[gridPos.row][gridPos.col] = GRID_CELL_DOOR;
        }
    }
}

CMiniMap::GridCoord CMiniMap::WorldToGrid(_float3 worldPos)
{
    int col = static_cast<int>((worldPos.x - m_MapMinX) / m_GridCellSize);
    int row = static_cast<int>((worldPos.z - m_MapMinZ) / m_GridCellSize);
    return { row, col };
}

_bool CMiniMap::IsValidGridCoord(const GridCoord& coord)
{
    return coord.row >= 0 && coord.row < m_GridRows &&
        coord.col >= 0 && coord.col < m_GridColumns;
}

void CMiniMap::DrawRectOnMiniMap(float x, float y, float width, float height, D3DCOLOR color)
{
    VERTEX* pVertices;
    m_pVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);

    pVertices[0] = { D3DXVECTOR3(x, y, 0), color };
    pVertices[1] = { D3DXVECTOR3(x + width, y, 0), color };
    pVertices[2] = { D3DXVECTOR3(x + width, y + height, 0), color };
    pVertices[3] = { D3DXVECTOR3(x, y + height, 0), color };

    m_pVertexBuffer->Unlock();

    // TRIANGLEFAN으로 그리기
    m_pGraphic_Device->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(VERTEX));
    m_pGraphic_Device->SetFVF(VERTEX::FVF);
    m_pGraphic_Device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
}

void CMiniMap::RenderStructureOnMiniMap(CGameObject* pObject)
{
    _float3 worldPos = dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
    _float2 miniMapPos = ConvertToMiniMapPos(worldPos);
    float iconSize = GetIconSizeForObject(pObject);

    // 예시로 흰색 아이콘으로 그림
    DrawBoxOnMiniMap(D3DXVECTOR2(miniMapPos.x, miniMapPos.y), D3DCOLOR_XRGB(255, 255, 255), iconSize);
}

_float CMiniMap::GetIconSizeForObject(CGameObject* pObject)
{
    // 기본 아이콘 크기 (미니맵 상에서의 기본 크기)
    float baseSize = 10.f;

    // 객체의 스케일을 가져옴
    _float3 scale = dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform")))->Compute_Scaled();

    // 예를 들어 x축의 스케일을 기준으로 크기 조정
    return baseSize * scale.x;
}

void CMiniMap::RenderPlayerOnMiniMap()
{
    if (m_pPlayer)
    {
        // 플레이어 위치 가져오기
        _float3 playerPos = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

        // 미니맵 좌표로 변환
        _float2 miniMapPos = ConvertToMiniMapPos(playerPos);

        // 플레이어를 빨간색 점으로 표시
        DrawBoxOnMiniMap(D3DXVECTOR2(miniMapPos.x, miniMapPos.y), D3DCOLOR_XRGB(255, 0, 0),5.f);
    }
}

void CMiniMap::RenderTestSquare()
{
    VERTEX vertices[4] = {
         { D3DXVECTOR3(100, 100, 0), 0xFFFF0000 },
         { D3DXVECTOR3(200, 100, 0), 0xFFFF0000 },
         { D3DXVECTOR3(200, 200, 0), 0xFFFF0000 },
         { D3DXVECTOR3(100, 200, 0), 0xFFFF0000 }
    };

    // 임시 버텍스 버퍼 생성
    LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;
    if (FAILED(m_pGraphic_Device->CreateVertexBuffer(sizeof(vertices), 0, (D3DFVF_XYZ | D3DFVF_DIFFUSE), D3DPOOL_MANAGED, &pVB, nullptr)))
    {
        return; // 버퍼 생성 실패 시 처리
    }

    // 버텍스 버퍼에 데이터 복사
    void* pData = nullptr;
    pVB->Lock(0, sizeof(vertices), (void**)&pData, 0);
    memcpy(pData, vertices, sizeof(vertices));
    pVB->Unlock();
    m_pGraphic_Device->SetFVF((D3DFVF_XYZ | D3DFVF_DIFFUSE));
    m_pGraphic_Device->SetStreamSource(0, pVB, 0, sizeof(VERTEX));

    // TRIANGLEFAN 방식으로 사각형(두 개의 삼각형)을 그립니다.
    m_pGraphic_Device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

    // 사용 후 버텍스 버퍼 해제
    pVB->Release();
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
    __super::Free();
    Safe_Release(m_pMiniMapSurface);
    Safe_Release(m_pMiniMapTexture);
    Safe_Release(m_pVertexBuffer);
    Safe_Release(m_pSprite); // 스프라이트 해제 추가
    //Safe_Release(m_pTransformCom);
    //Safe_Release(m_pMaterialCom);
}
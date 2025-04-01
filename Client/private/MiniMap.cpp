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
    :CGameObject(pGraphic_Device)
{
}

CMiniMap::CMiniMap(const CMiniMap& Prototype)
    : CGameObject(Prototype),
    m_pMiniMapSurface(Prototype.m_pMiniMapSurface),
    m_pMiniMapTexture(Prototype.m_pMiniMapTexture)
{
    Safe_AddRef(m_pMiniMapSurface);
    Safe_AddRef(m_pMiniMapTexture);
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
    // 1. 원래 렌더 타겟 저장
    LPDIRECT3DSURFACE9 pBackBuffer = nullptr;
    m_pGraphic_Device->GetRenderTarget(0, &pBackBuffer);

    // 2. 렌더 타겟을 미니맵 텍스처로 변경
    m_pGraphic_Device->SetRenderTarget(0, m_pMiniMapSurface);

    // 3. 미니맵 텍스처 클리어 (배경색을 어두운 회색으로 설정 - 복도 표현)
    m_pGraphic_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(50, 50, 50), 1.0f, 0);

    // 4. 미니맵 내용 그리기
    SetUp_RenderState();

    // 그리드 기반으로 미니맵 렌더링
    RenderGridMap();

    // 플레이어 위치 표시
    RenderPlayerOnMiniMap();

    Release_RenderState();

    // 5. 원래 렌더 타겟으로 복원
    m_pGraphic_Device->SetRenderTarget(0, pBackBuffer);
    Safe_Release(pBackBuffer);

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

        m_pSprite->Draw(m_pMiniMapTexture, NULL, NULL, NULL, D3DCOLOR_ARGB(200, 255, 255, 255)); // 약간 투명하게

        // 스프라이트 종료
        m_pSprite->End();
    }
    return S_OK;
}

HRESULT CMiniMap::SetUp_RenderState()
{
    m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &m_OldProjMatrix);
    m_pGraphic_Device->GetTransform(D3DTS_VIEW, &m_OldViewMatrix);

    // 렌더 상태 설정
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

    // 뷰 행렬 설정
    D3DXMATRIX matView;
    D3DXMatrixIdentity(&matView);
    D3DXVECTOR3 eye(0.0f, 0.0f, -10.0f);
    D3DXVECTOR3 at(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMatrixLookAtLH(&matView, &eye, &at, &up);
    m_pGraphic_Device->SetTransform(D3DTS_VIEW, &matView);

    // 직교 투영 행렬 설정
    D3DXMATRIX matOrtho;
    D3DXMatrixOrthoLH(&matOrtho, MINIMAP_SIZE, MINIMAP_SIZE, 0.1f, 1000.0f);
    m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOrtho);

    return S_OK;
}

HRESULT CMiniMap::Release_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    m_pGraphic_Device->SetRenderState(D3DRS_LIGHTING, TRUE); // 조명 상태 복원
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
    float minX = FLT_MAX, minZ = FLT_MAX;
    float maxX = FLT_MIN, maxZ = FLT_MIN;

    // 벽과 문의 위치를 전부 확인
    for (const auto& structure : m_StructureList)
    {
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

    m_MapWidth = maxX - minX;
    m_MapHeight = maxZ - minZ;

    // 최소 좌표 저장 (좌표 변환에 사용)
    m_MapMinX = minX;
    m_MapMinZ = minZ;

    // 그리드 크기 계산
    m_GridColumns = ceil(m_MapWidth / m_GridCellSize);
    m_GridRows = ceil(m_MapHeight / m_GridCellSize);
}

_float2 CMiniMap::ConvertToMiniMapPos(_float3 vPos)
{
    // 월드 좌표 -> 미니맵 좌표 변환
    float scaleX = 256.0f / m_MapWidth; // 미니맵의 너비를 월드의 너비로 나눔
    float scaleY = 256.0f / m_MapHeight; // 미니맵의 높이를 월드의 높이로 나눔

    float miniMapPosX = (vPos.x - m_MapMinX) * scaleX; // 월드 좌표를 미니맵 좌표로 변환
    float miniMapPosY = (vPos.z - m_MapMinZ) * scaleY; // 월드 좌표를 미니맵 좌표로 변환

    return _float2(miniMapPosX, miniMapPosY);
}

void CMiniMap::DrawBoxOnMiniMap(D3DXVECTOR2 pos,D3DCOLOR color)
{
    VERTEX* pVertices;
    m_pVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);

    float size = 2.0f; // 박스 크기

    pVertices[0] = { D3DXVECTOR3(pos.x - size, pos.y - size, 0), color };
    pVertices[1] = { D3DXVECTOR3(pos.x + size, pos.y - size, 0), color };
    pVertices[2] = { D3DXVECTOR3(pos.x + size, pos.y + size, 0), color };
    pVertices[3] = { D3DXVECTOR3(pos.x - size, pos.y + size, 0), color };

    m_pVertexBuffer->Unlock();

    // TRIANGLELIST에서 TRIANGLEFAN으로 변경
    m_pGraphic_Device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
}

void CMiniMap::UpdateGridMap()
{
}

void CMiniMap::RenderGridMap()
{
    float cellSizeX = static_cast<float>(MINIMAP_SIZE) / m_GridColumns;
    float cellSizeY = static_cast<float>(MINIMAP_SIZE) / m_GridRows;

    for (int row = 0; row < m_GridRows; ++row)
    {
        for (int col = 0; col < m_GridColumns; ++col)
        {
            // 미니맵 상의 위치 계산
            float x = col * cellSizeX;
            float y = row * cellSizeY;

            // 셀 타입에 따라 색상 결정
            D3DCOLOR color;
            switch (m_GridMap[row][col])
            {
            case GRID_CELL_WALL:
                color = D3DCOLOR_XRGB(0, 0, 0); // 검은색 - 벽
                break;
            case GRID_CELL_DOOR:
                color = D3DCOLOR_XRGB(150, 150, 150); // 회색 - 문
                break;
            case GRID_CELL_CORRIDOR:
            default:
                continue; // 복도는 배경색(어두운 회색)으로 이미 그려져 있으므로 스킵
            }

            // 셀 그리기
            DrawRectOnMiniMap(x, y, cellSizeX, cellSizeY, color);
        }
    }
}

void CMiniMap::InitializeGridMap()
{ // 그리드 셀 크기 설정 (실제 월드 단위)
    m_GridCellSize = 2.0f; // 적절한 크기로 조정

    // 그리드 행과 열 수 계산
    m_GridColumns = ceil(m_MapWidth / m_GridCellSize);
    m_GridRows = ceil(m_MapHeight / m_GridCellSize);

    // 그리드 맵 메모리 할당
    m_GridMap.resize(m_GridRows);
    for (int i = 0; i < m_GridRows; ++i)
    {
        m_GridMap[i].resize(m_GridColumns, GRID_CELL_CORRIDOR); // 기본값은 복도
    }

    // 월드 좌표를 그리드 좌표로 변환하여 벽과 문 배치
    for (const auto& structure : m_StructureList)
    {
        if (structure->Get_Tag().find(L"Floor") != _wstring::npos) continue;
        _float3 pos = dynamic_cast<CTransform*>(structure->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
        GridCoord gridPos = WorldToGrid(pos);

        // 그리드 범위 체크
        if (IsValidGridCoord(gridPos))
        {
            m_GridMap[gridPos.row][gridPos.col] = GRID_CELL_WALL;
        }
    }

    for (const auto& door : m_DoorList)
    {
        _float3 pos = dynamic_cast<CTransform*>(door->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
        GridCoord gridPos = WorldToGrid(pos);

        // 그리드 범위 체크
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

void CMiniMap::RenderPlayerOnMiniMap()
{
    if (m_pPlayer)
    {
        // 플레이어 위치 가져오기
        _float3 playerPos = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);

        // 미니맵 좌표로 변환
        _float2 miniMapPos = ConvertToMiniMapPos(playerPos);

        // 플레이어를 빨간색 점으로 표시
        DrawBoxOnMiniMap(D3DXVECTOR2(miniMapPos.x, miniMapPos.y), D3DCOLOR_XRGB(255, 0, 0));
    }
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
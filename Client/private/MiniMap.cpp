#include "MiniMap.h"
#include "Player.h"
#include <Transform.h>
#include <StructureManager.h>

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
    m_pGraphic_Device->CreateTexture(256, 256, 1, D3DUSAGE_RENDERTARGET,
        D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
        &m_pMiniMapTexture, NULL);
    m_pMiniMapTexture->GetSurfaceLevel(0, &m_pMiniMapSurface);

    //// 기본 바닥을 흰색으로 칠하기
    //D3DLOCKED_RECT lockedRect;
    //m_pMiniMapSurface->LockRect(&lockedRect, NULL, 0);

    //DWORD* pPixel = (DWORD*)lockedRect.pBits;
    //for (int y = 0; y < 256; ++y)
    //{
    //    for (int x = 0; x < 256; ++x)
    //    {
    //        pPixel[x + y * (lockedRect.Pitch / 4)] = D3DCOLOR_XRGB(255, 255, 255); // 흰색 바닥
    //    }
    //}

    //m_pMiniMapSurface->UnlockRect();
    return S_OK;
}

HRESULT CMiniMap::Initialize(void* pArg)
{
    m_pGraphic_Device->CreateVertexBuffer(
        4 * sizeof(VERTEX), // 정점 4개
        0,
        VERTEX::FVF,
        D3DPOOL_MANAGED,
        &m_pVertexBuffer,
        NULL
    );
    m_DoorList =  CStructureManager::Get_Instance()->Get_Doors();
    m_StructureList =  CStructureManager::Get_Instance()->Get_Structures();


    m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_Player"));
    if (!m_pPlayer)
    {
        m_pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, L"Layer_Player"));
    }

    if (!m_pPlayer)
    {
        return E_FAIL;
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
    SetUp_RenderState();
    // 1. 벽을 검은색으로 칠하기
    for (const auto& structure : m_StructureList)
    {
        D3DXVECTOR2 miniMapPos = ConvertToMiniMapPos(dynamic_cast<CTransform*>(structure->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION));
        DrawBoxOnMiniMap(miniMapPos, D3DCOLOR_XRGB(0, 0, 0)); // 검은색 벽
    }

    // 2. 문을 회색으로 칠하기
    for (const auto& door : m_DoorList)
    {
        D3DXVECTOR2 miniMapPos = ConvertToMiniMapPos(dynamic_cast<CTransform*>(door->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION));
        DrawBoxOnMiniMap(miniMapPos, D3DCOLOR_XRGB(150, 150, 150)); // 회색 문
    }

    // 3. 플레이어 위치 표시
    D3DXVECTOR2 playerMiniMapPos = ConvertToMiniMapPos(dynamic_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION));
    DrawBoxOnMiniMap(playerMiniMapPos,D3DCOLOR_XRGB(0, 255, 0)); // 초록색 플레이어
    Release_RenderState();
    return S_OK;
}

HRESULT CMiniMap::SetUp_RenderState()
{
    m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &m_OldProjMatrix);

    // 직교 투영 행렬 설정
    D3DXMATRIX matOrtho;
    D3DXMatrixOrthoLH(&matOrtho, 100.0f, 100.0f, 0.1f, 1000.0f); // 크기는 미니맵 크기에 맞게 조절
    m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &matOrtho);
    return S_OK;
}

HRESULT CMiniMap::Release_RenderState()
{
    m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_OldProjMatrix);
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
}

_float2 CMiniMap::ConvertToMiniMapPos(_float3 vPos)
{
    float scale = 0.1f; // 월드 크기에 맞춰 조절
    return _float2(vPos.x * scale, vPos.z * scale);
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

    m_pGraphic_Device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
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
    //Safe_Release(m_pTransformCom);
    //Safe_Release(m_pMaterialCom);
}
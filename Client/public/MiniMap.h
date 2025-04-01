#pragma once
#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CTransform;
//class CTexture;
class CVIBuffer;
class CMaterial;
END

BEGIN(Client)
class CMiniMap final : public CGameObject
{
public:
    struct VERTEX
    {
        D3DXVECTOR3 position; // 3D ÁÂÇ¥
        D3DCOLOR color;       // »ö»ó

        static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
    };
private:
    CMiniMap(LPDIRECT3DDEVICE9 pGraphic_Device);
    CMiniMap(const CMiniMap& Prototype);
    virtual ~CMiniMap() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    HRESULT SetUp_RenderState();
    HRESULT Release_RenderState();
    HRESULT Ready_Components();

private:
    void CalculateMapSize();
    _float2 ConvertToMiniMapPos(_float3 vPos);
    void DrawBoxOnMiniMap(D3DXVECTOR2 pos, D3DCOLOR color);
private:
    list<class CDoor*> m_DoorList;
    list<class CStructure*> m_StructureList;
    class CPlayer* m_pPlayer = { nullptr };
    LPDIRECT3DTEXTURE9 m_pMiniMapTexture = {nullptr};  // ¹Ì´Ï¸Ê ·»´õ Å¸°Ù
    LPDIRECT3DSURFACE9 m_pMiniMapSurface = {nullptr};
    LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer = { nullptr };
    // ½ºÇÁ¶óÀÌÆ® °´Ã¼ Ãß°¡
    LPD3DXSPRITE m_pSprite = nullptr;
    // ¹Ì´Ï¸Ê Ãâ·Â À§Ä¡ ¹× Å©±â
    RECT m_MiniMapRect;

    _float m_MapWidth{0.f};
    _float m_MapHeight{0.f};
    _float m_MapMinX{0.f };
    _float m_MapMinZ{ 0.f };
    _float4x4 m_OldProjMatrix{};
    _float4x4 m_OldViewMatrix{};
    _int m_iGridX = 5;
    _int m_iGridY = 10;
    bool m_bGrids[5][10];

public:
    static CMiniMap* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
    virtual json Serialize() override;
    virtual void Deserialize(const json& j) override;
};
END
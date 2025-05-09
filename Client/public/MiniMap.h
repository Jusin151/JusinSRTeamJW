﻿#pragma once
#include "Client_Defines.h"
#include "GameObject.h"


BEGIN(Engine)
class CMaterial;
class CShader;
class CTransform;
class CVIBuffer;
class CMaterial;
class CTexture;
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

    struct GridCoord
    {
        int row;
        int col;
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
    void RenderPlayerOnMiniMap();
    void RenderHellBossOnMiniMap();
private:
    list<class CDoor*> m_DoorList;
    list<class CStructure*> m_StructureList;
    class CGameObject* m_pHellboss{ nullptr };
    class CPlayer* m_pPlayer = { nullptr };
    class CCamera_FirstPerson* m_pCamera = { nullptr };
private:
    LPDIRECT3DTEXTURE9 m_pMiniMapTexture = {nullptr};  // ¹Ì´Ï¸Ê ·»´õ Å¸°Ù
    LPDIRECT3DSURFACE9 m_pMiniMapSurface = {nullptr};
    LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer = { nullptr };
    LPD3DXSPRITE m_pSprite = nullptr;
private:
    CTexture* m_pTextureCom = { nullptr };
    CShader* m_pShaderCom = { nullptr };
    CMaterial* m_pMaterialCom = { nullptr };
    CTransform* m_pTransformCom = { nullptr };
private:
    _bool m_bIsOriginal = { true };
    _float4x4 m_OldProjMatrix{};
    _float4x4 m_OldViewMatrix{};
    _float4x4 m_matMiniMapView;
    _float4x4 m_matMiniMapProj;
    _bool m_bStaticMapRendered = false;
    

public:
    static CMiniMap* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
    virtual json Serialize() override;
    virtual void Deserialize(const json& j) override;
};
END
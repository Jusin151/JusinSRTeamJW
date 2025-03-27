#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;  // UI는 사각형으로 할듯
END

BEGIN(Client)

class CImage  : public CGameObject
{
public:
    typedef struct tagImage
    {
        _float2 vPos{};
        _float2 vSize{};
        _float fAlpha{};
        _uint TextureImageNum{};
        _wstring TextureKey{};
    }Image_DESC;
public:
    CImage(LPDIRECT3DDEVICE9 pGraphic_Device);
    CImage(const CImage& Prototype);
    virtual ~CImage() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render()override;
    virtual HRESULT Ready_Components();


public:
   static CImage* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    CGameObject* Clone(void* pArg);
    void Free();

private:
    Image_DESC m_Image_INFO;

protected:
    CTexture* m_pTextureCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CTransform* m_pTransformCom = { nullptr };
};
END

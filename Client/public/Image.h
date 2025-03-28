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
    enum IMAGE_TYPE
    {
        WEAPON_ICON,
        NORMAL_IMAGE
    };

    typedef struct tagImage
    {
        _float2 vPos{};
        _float2 vSize{};
        _float fAlpha{};
        IMAGE_TYPE IMAGE_TYPE{};
        _uint TextureImageNum{}; // 몇번째 이미지인지 (폴더확인)
        wstring WeaponTag = {}; //아이템매니저에 등록할때의 Wstring과 똑같이 넣어주세용
        _wstring TextureKey{}; // 텍스쳐키
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
    void Select_WeaponIcon(_bool type)
    {
        if (type)
            m_pTransformCom->Set_Scale(m_Image_INFO.vSize.x * 1.5f, m_Image_INFO.vSize.y * 1.5f, 1.f);
        else
            m_pTransformCom->Set_Scale(m_Image_INFO.vSize.x, m_Image_INFO.vSize.y, 1.f);
    }
   
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

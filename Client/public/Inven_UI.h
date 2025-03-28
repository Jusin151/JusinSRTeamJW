#pragma once
#include "Client_Defines.h"
#include "UI_Base.h"
#include "Transform.h"
#include "Image.h"
#include "Image_Manager.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;  // UI는 사각형으로 할듯
END

BEGIN(Client)

class CInven_UI : public CUI_Base
{
public:
  
    typedef struct tagImage
    {
        _float2 vPos{};
        _float2 vSize{};
        _float fAlpha{};
        _wstring TextureKey{};
    }Image_DESC;
public:
    CInven_UI(LPDIRECT3DDEVICE9 pGraphic_Device);
    CInven_UI(const CInven_UI& Prototype);
    virtual ~CInven_UI() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render()override;
    virtual HRESULT Ready_Components();


    void Add_WeaponIcon(const wstring& WeaponIconTag) //인벤의 Add_Weapon에서 호출됨
    {
     CImage* pImage = CImage_Manager::GetInstance()->Get_WeaponIcon(WeaponIconTag);
 
     if (pImage != nullptr)
     {
         m_pWeaponIcon.push_back(pImage);
     }
    }

    void WeaponIcon_isActive(_uint Index)
    {
        for (auto& it : m_pWeaponIcon)
        {
            it->Select_WeaponIcon(false);
        }

        if(m_pWeaponIcon[Index]!=nullptr)
         m_pWeaponIcon[Index]->Select_WeaponIcon(true);

        return; 
    }

    void Inven_OnOff(_bool type)
    {
        m_bRender = type; // 인벤창 off

        if (m_bRender)
        {
            for (auto& it : m_pWeaponIcon)
            {
                it->SetActive(true);
            }
        }
        else
        {      
            for (auto& it : m_pWeaponIcon)
            {
                it->SetActive(false);
            }
            
        }
    }
public:
    static CInven_UI* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    CGameObject* Clone(void* pArg);
    void Free();

private: 
    Image_DESC m_Image_INFO;
    vector<CImage*> m_pWeaponIcon{};
    CImage* asdasd={ nullptr };
    unordered_map<wstring, CImage*> m_Map_pWeaponIcon={};


private: // 인빈 이미지에 관련된 변수들
    _bool m_bRender={false};
};
END
